#include "Sic_ProcessView.h"

Sic_ProcessView::Sic_ProcessView()
{
	if (!m_processview) {
		m_processview = new Processview;
	}
	m_sysInfo->m_ProcessView = m_processview;
	SetDebugFlag = false;
}

Sic_ProcessView::~Sic_ProcessView()
{
	if (m_processview) {
		delete m_processview;
		m_processview = nullptr;
	}
	m_sysInfo->m_ProcessView = nullptr;
}

void Sic_ProcessView::Execute(void)
{
	GetProcessDetils();
}

std::string Sic_ProcessView::GetErrors(void)
{
	return std::string();
}

void Sic_ProcessView::GetProcessDetils(void)
{
	std::vector<ProcessObj> result;
	HANDLE hProcessSnap;
	HANDLE hToken;
	PROCESSENTRY32 stcPe32 = { 0 };
	BOOLEAN isSucess = FALSE;
	stcPe32.dwSize = sizeof(PROCESSENTRY32);
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		m_processview->Status |= ProcessViewStatus::FAILED_TO_CREATE_SNAPSHOT;
		return;
	}
	if (!Process32First(hProcessSnap, &stcPe32)) {
		m_processview->Status |= ProcessViewStatus::FAILED_TO_START_FIRSTFUNC;
		CloseHandle(hProcessSnap);
		return;
	}
	if (SetDebugFlag)
	{
		if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken))
		{
			if (GetLastError() == ERROR_NO_TOKEN)
			{
				if (ImpersonateSelf(SecurityImpersonation)) {
					if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken))
					{
						m_processview->Status |= ProcessViewStatus::FAILED_TO_OPENTHREADTOKEN;
					}
				}
				else {
					m_processview->Status |= ProcessViewStatus::FAILED_TO_IMPERSONATESELF;
				}
			}
		}
		if (m_processview->Status == ProcessViewStatus::PVS_NO_ERROR)
		{
			SetPrivilege(hToken, SE_DEBUG_NAME, TRUE);
		}
	}
	int ompNumThreads = SicMpiPool::getInstance()->GetOmpThreadsNumbers();
	std::map<int, PROCESSENTRY32> tmp;
	for (int i = 0; i < ompNumThreads; i++)
	{
		PROCESSENTRY32 tpc32 = { 0 };
		tpc32.dwSize = sizeof(PROCESSENTRY32);
		tmp[i] = tpc32;
	}
	std::mutex resultLocker;
	auto ParseProcNextFunc = [&](const PROCESSENTRY32& _procentry) {
		ProcessObj obj;
		obj.parentProcessId = _procentry.th32ParentProcessID;
		obj.processId = _procentry.th32ProcessID;
		wmemcpy(obj.exeFile, _procentry.szExeFile, MAX_PATH);
		obj.Status = ProcessViewDetilStatus::PVDS_NO_ERROR;
		HANDLE hProcess;
		PROCESS_MEMORY_COUNTERS pmc;
		hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, _procentry.th32ProcessID);
		if (hProcess) {
			DWORD handleCount;
			ZeroMemory(&handleCount, sizeof(DWORD));
			if (GetProcessHandleCount(hProcess, &obj.HandleCount)) {
#pragma omp atomic
				m_processview->HandleCount += obj.HandleCount;
			}
			else {
				obj.Status |= ProcessViewDetilStatus::FAILED_QUERY_HANDLE_COUNT;
			}
			if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
			{
				obj.parseFrom(pmc);
			}
			else {
				obj.Status |= ProcessViewDetilStatus::FAILED_QUERY_MEMORY_INFO;
			}

			if (GetProcessTimes(hProcess, &obj.lpCreationTime, &obj.lpExitTime, &obj.lpKernelTime, &obj.lpUserTime))
			{
				if(m_sysInfo->m_OverView)
					obj.cpuUsageTime = (static_cast<double>(obj.FileTimeToInt64(obj.lpKernelTime)) + obj.FileTimeToInt64(obj.lpUserTime)) / (m_sysInfo->m_OverView->m_cpuInfo->CPUCoreCount * 1.0);
			}
			else {
				obj.Status |= ProcessViewDetilStatus::FAILED_QUERY_TIMES_INFO;
			}
			CloseHandle(hProcess);
		}
		else {
			obj.Status |= ProcessViewDetilStatus::FAILED_OPEN_PROCESS;
		}
		resultLocker.lock();
		result.push_back(obj);
		resultLocker.unlock();
	};
	ParseProcNextFunc(stcPe32);
#pragma omp parallel for num_threads(ompNumThreads) shared(hProcessSnap)
	{
		int index = omp_get_thread_num();
#pragma omp critical
		for (; Process32Next(hProcessSnap, &tmp[index]); )
		{
#pragma omp atomic
			m_processview->ThreadsCount += tmp[index].cntThreads;
			ParseProcNextFunc(tmp[index]);
		}
	}
	CloseHandle(hProcessSnap);
	if (SetDebugFlag && hToken)
	{
		SetPrivilege(hToken, SE_DEBUG_NAME, FALSE);
	}
	m_processview->ProcessCount = result.size();
	m_processview->ProcessDetils.clear();
	m_processview->ProcessDetils = std::vector<ProcessObj>(result);
}

Sic_ProcessView* Sic_ProcessView::getInstance()
{
	m_mutex_process->lock();
	if (instance == nullptr) {
		instance = new Sic_ProcessView;
	}
	m_mutex_process->unlock();
	return instance;
}

BOOL Sic_ProcessView::SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
	TOKEN_PRIVILEGES tp; // 该结构包含一个数组，数据组的每个项指明了权限的类型和要进行的操作
	LUID luid;

	// 查找
	if (!LookupPrivilegeValue(
		NULL,            // 系统的名字,null,在本地系统上查找权限 lookup privilege on local system
		lpszPrivilege,   // 要找的权限名 privilege to lookup 
		&luid))        // 通过指针返回权限的LUID receives LUID of privilege
	{
		printf("LookupPrivilegeValue error: %u\n", GetLastError());
		return FALSE;
	}

	tp.PrivilegeCount = 1;    // 要修改的特权数目
	tp.Privileges[0].Luid = luid;    // 代表不同特权类型
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// 调整访问令牌的权限    Enable the privilege or disable all privileges.
	if (!AdjustTokenPrivileges(
		hToken,// OpenProcessToken第三个指针参数传出的访问令牌的句柄
		FALSE, // 是否禁用所有所有的特权
		&tp,    // 指明要修改的权限
		sizeof(TOKEN_PRIVILEGES),    // PreviousState的长度
		NULL,   // 存放修改前的访问权限的信息，可空
		NULL))    // 实际PreviousState结构返回的大小
	{
		printf("AdjustTokenPrivileges error: %u\n", GetLastError());
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		printf("令牌在NewState参数中没有这个或这些权限 \n");
		return FALSE;
	}

	return TRUE;
}