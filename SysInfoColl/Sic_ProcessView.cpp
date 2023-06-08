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
	TOKEN_PRIVILEGES tp; // �ýṹ����һ�����飬�������ÿ����ָ����Ȩ�޵����ͺ�Ҫ���еĲ���
	LUID luid;

	// ����
	if (!LookupPrivilegeValue(
		NULL,            // ϵͳ������,null,�ڱ���ϵͳ�ϲ���Ȩ�� lookup privilege on local system
		lpszPrivilege,   // Ҫ�ҵ�Ȩ���� privilege to lookup 
		&luid))        // ͨ��ָ�뷵��Ȩ�޵�LUID receives LUID of privilege
	{
		printf("LookupPrivilegeValue error: %u\n", GetLastError());
		return FALSE;
	}

	tp.PrivilegeCount = 1;    // Ҫ�޸ĵ���Ȩ��Ŀ
	tp.Privileges[0].Luid = luid;    // ����ͬ��Ȩ����
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// �����������Ƶ�Ȩ��    Enable the privilege or disable all privileges.
	if (!AdjustTokenPrivileges(
		hToken,// OpenProcessToken������ָ����������ķ������Ƶľ��
		FALSE, // �Ƿ�����������е���Ȩ
		&tp,    // ָ��Ҫ�޸ĵ�Ȩ��
		sizeof(TOKEN_PRIVILEGES),    // PreviousState�ĳ���
		NULL,   // ����޸�ǰ�ķ���Ȩ�޵���Ϣ���ɿ�
		NULL))    // ʵ��PreviousState�ṹ���صĴ�С
	{
		printf("AdjustTokenPrivileges error: %u\n", GetLastError());
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		printf("������NewState������û���������ЩȨ�� \n");
		return FALSE;
	}

	return TRUE;
}