#include "SysInfoColl.h"


SysInfoColl::SysInfoColl()
{
	m_mct = MonitorColType::USE_DC;
	GetSystemFoudationInfo();
	RefreshSystemDetils();
}

SysInfoColl::~SysInfoColl()
{
}

void SysInfoColl::GetDeviceList(std::vector<DeviceObj>& _installedDev, std::vector<DeviceObj>& _uninstalledDev)
{
	std::vector<DeviceObj> alldevices = GetAllDevices();
	std::set<std::wstring> instanceIds;
	for (int i = 0; i < alldevices.size(); i++)
	{
		instanceIds.insert(alldevices[i].instanceId);
	}
	for (int i = 0; i < m_devices.size(); i++)
	{
		if (instanceIds.find(m_devices[i].instanceId) == instanceIds.end())
		{
			_uninstalledDev.push_back(m_devices[i]);
		}
	}
	instanceIds.clear();
	for (int i = 0; i < m_devices.size(); i++)
	{
		instanceIds.insert(m_devices[i].instanceId);
	}
	for (int i = 0; i < alldevices.size(); i++)
	{
		if (instanceIds.find(alldevices[i].instanceId) == instanceIds.end())
		{
			_installedDev.push_back(alldevices[i]);
		}
	}
	m_devices.clear();
	m_devices = std::vector<DeviceObj>(alldevices);
}

std::vector<EventLogObj> SysInfoColl::GetSystemEventLog(unsigned long timeTick, EventLogEnum eventNamet)
{
	std::vector< EventLogObj> result;
	m_sysinfo.getsystemeventlogwarning = GetEventLogWarning::GELW_NO_ERROR;
	EVENTLOG_FULL_INFORMATION evntLogInfo;
	DWORD dwByteRequd, cbSize = 0, dwBytesToRead = MAX_RECORD_BUFFER_SIZE, dwBytesRead, dwMinimumBytesNeeded, numRecord;
	CONST wchar_t* pEventTypeNames[] = { L"Error", L"Warning", L"Informational", L"Audit Success", L"Audit Failure" };
	PBYTE pBuffer, currentData, endRecord;
	HANDLE eventHandle = NULL;
	switch (eventNamet)
	{
	case DT::EV_SYSTEM:
		eventHandle = OpenEventLog(NULL, L"System");
		break;
	case DT::EV_APPLICATION:
		eventHandle = OpenEventLog(NULL, L"Application");
		break;
	case DT::EV_SECURITY:
		eventHandle = OpenEventLog(NULL, L"Security");
		break;
	}
	if (eventHandle == INVALID_HANDLE_VALUE)
	{
		m_sysinfo.getsystemeventloglasterror = GetLastError();
		m_sysinfo.getsystemeventlogwarning = GetEventLogWarning::FAILED_OPEN_EVENT_LOG;
	}
	else
	{
		pBuffer = (PBYTE)malloc(MAX_RECORD_BUFFER_SIZE);
		if (pBuffer == NULL)
		{
			m_sysinfo.getsystemeventlogwarning = m_sysinfo.getsystemeventlogwarning | GetEventLogWarning::FAILED_NO_ENOUGH_MEMORY;
			CloseEventLog(eventHandle);
		}
		else
		{
			//GetEventLogInformation(eventHandle,EVENTLOG_FULL_INFO,&pBuffer,cbSize,&dwByteRequd);
			ReadEventLog(eventHandle, EVENTLOG_SEQUENTIAL_READ | EVENTLOG_BACKWARDS_READ, 0, pBuffer, dwBytesToRead, &dwBytesRead, &dwMinimumBytesNeeded);
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				pBuffer = (PBYTE)realloc(pBuffer, dwMinimumBytesNeeded);
				if (pBuffer == NULL)
				{
					m_sysinfo.getsystemeventloglasterror = GetLastError();
					m_sysinfo.getsystemeventlogwarning = m_sysinfo.getsystemeventlogwarning | GetEventLogWarning::FAILED_REALLOC_MEMORY;
					CloseEventLog(eventHandle);
				}
				else
				{
					dwBytesToRead = dwMinimumBytesNeeded;
					ReadEventLog(eventHandle, EVENTLOG_SEQUENTIAL_READ | EVENTLOG_BACKWARDS_READ, 0, pBuffer, dwBytesToRead, &dwBytesRead, &dwMinimumBytesNeeded);
				}
			}
			GetNumberOfEventLogRecords(eventHandle, &numRecord);
			std::cout << numRecord << "\n";
			endRecord = pBuffer + dwBytesRead;
			PBYTE pRecord = pBuffer;
			while (pRecord < endRecord)
			{
				EventLogObj eventObj;
				auto record_data = (PEVENTLOGRECORD)pRecord;
				if (record_data->TimeGenerated < timeTick) {
					break;
				}
				GetTimestamp(record_data->TimeGenerated, eventObj.TimeStamp);
				eventObj.statusCode = record_data->EventID & 0xFFFF;
				eventObj.recNumber = record_data->RecordNumber;
				eventObj.eventId = record_data->EventID;
				eventObj.normalInfo = std::wstring((LPWSTR)(LPBYTE)pRecord + (record_data->StringOffset / 2));
				switch (record_data->EventType)
				{
				case EVENTLOG_ERROR_TYPE:
					eventObj.eventType = std::wstring(pEventTypeNames[0]);
					break;
				case EVENTLOG_WARNING_TYPE:
					eventObj.eventType = std::wstring(pEventTypeNames[1]);
					break;
				case EVENTLOG_INFORMATION_TYPE:
					eventObj.eventType = std::wstring(pEventTypeNames[2]);
					break;
				case EVENTLOG_AUDIT_SUCCESS:
					eventObj.eventType = std::wstring(pEventTypeNames[3]);
					break;
				case EVENTLOG_AUDIT_FAILURE:
					eventObj.eventType = std::wstring(pEventTypeNames[4]);
					break;
				default:
					eventObj.eventType = L"Unknow";
					break;
				}

				if (record_data->DataLength > 0)
				{
					eventObj.recData = std::wstring((LPWSTR)(pRecord + record_data->DataOffset));
				}
				result.push_back(eventObj);
				pRecord += record_data->Length;
			}

			if (eventHandle)
				CloseEventLog(eventHandle);

			if (pBuffer)
				free(pBuffer);
		}
	}
	return result;
}

void SysInfoColl::RefreshSystemDetils()
{
	//获取内存信息
	{
		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof(statex);
		GlobalMemoryStatusEx(&statex);

		m_sysinfo.meminfo.memoryUsePercent = statex.dwMemoryLoad;
		m_sysinfo.meminfo.physicalMemory.first = statex.ullTotalPhys / DIV / DIV;
		m_sysinfo.meminfo.physicalMemory.second = statex.ullAvailPhys / DIV / DIV;
		m_sysinfo.meminfo.pagingMemory.first = statex.ullTotalPageFile / DIV / DIV;
		m_sysinfo.meminfo.pagingMemory.second = statex.ullAvailPageFile / DIV / DIV;
		m_sysinfo.meminfo.virtualMemory.first = statex.ullTotalVirtual / DIV / DIV;
		m_sysinfo.meminfo.virtualMemory.second = statex.ullAvailVirtual / DIV / DIV;
		m_sysinfo.meminfo.extendedMemory = statex.ullAvailExtendedVirtual / DIV / DIV;
	}
	//屏幕分辨率及缩放比
	{
		switch (m_mct)
		{
		case (MonitorColType::USE_DC):
		{
			HDC hDC = ::GetDC(NULL);
			m_sysinfo.monitor_x = ::GetDeviceCaps(hDC, DESKTOPHORZRES);
			m_sysinfo.monitor_y = ::GetDeviceCaps(hDC, DESKTOPVERTRES);
			auto x = GetDeviceCaps(hDC, LOGPIXELSX);
			m_sysinfo.monitor_zoom = (x * 1.0) / (m_sysinfo.monitor_x * 1.0);
			::ReleaseDC(NULL, hDC);
			break;
		}
		case (MonitorColType::USE_SYS):
		{
			m_sysinfo.monitor_x = GetSystemMetrics(SM_CXFULLSCREEN);
			m_sysinfo.monitor_y = GetSystemMetrics(SM_CYFULLSCREEN);
			HWND hd = GetDesktopWindow();
			HINSTANCE userdll = NULL;
			userdll = LoadLibrary(L"user32.dll");
			if (userdll) {
				GetDpiForWindow_USER32 GetDpiForWindowFunc = (GetDpiForWindow_USER32)GetProcAddress(userdll, "GetDpiForWindow");
				if (GetDpiForWindowFunc) {
					int dpi = GetDpiForWindowFunc(hd);
					switch (dpi) {
					case 96:
						m_sysinfo.monitor_zoom = 1;
						break;
					case 120:
						m_sysinfo.monitor_zoom = 1.25;
						break;
					case 144:
						m_sysinfo.monitor_zoom = 1.5;
						break;
					case 192:
						m_sysinfo.monitor_zoom = 2;
						break;
					}
				}
				FreeLibrary(userdll);
			}
			break;
		}
		}
	}
	//获取用户名
	{
		DWORD  bufCharCount = INFO_BUFFER_SIZE;
		GetUserName(m_sysinfo.username, &bufCharCount);
	}
	//获取网络信息
	{
		WSAData data;
		if (WSAStartup(MAKEWORD(1, 1), &data) != 0) {
			strcpy(m_sysinfo.host_name, "Failed to start wsa");
		}
		else {
			char host[255];
			struct hostent* p = gethostbyname(NULL);
			if (gethostname(host, sizeof(host)) == SOCKET_ERROR) {
				strcpy(m_sysinfo.host_name, host);
			}
			else {
				strcpy(m_sysinfo.host_name, "Failed to get hostname");
			}
			if (p)
			{
				for (int i = 0; p->h_addr_list[i] != 0; i++) {
					struct in_addr in;
					memcpy(&in, p->h_addr_list[i], sizeof(struct in_addr));
					m_sysinfo.networkIps.push_back(inet_ntoa(in));
				}
			}
			else {

			}
		}
		WSACleanup();
	}
	//获取GPU信息 by dx
	{
		m_sysinfo.getgpuinfowarning = GetGPUInfoWarning::GGIW_NO_ERROR;
		m_sysinfo.DirectXVersion = GetDxVersion();
		IDXGIFactory* pFactory;
		IDXGIAdapter* pAdapter;
		std::vector <IDXGIAdapter*> vAdapters;
		int iAdapterNum = 0;
		
		HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));

		if (FAILED(hr)) {
			m_sysinfo.getgpuinfowarning = GetGPUInfoWarning::CREATE_DXGIFACTORY_FAILED;
			return;
		}

		while (pFactory->EnumAdapters(iAdapterNum, &pAdapter) != DXGI_ERROR_NOT_FOUND)
		{
			vAdapters.push_back(pAdapter);
			++iAdapterNum;
		}
		m_sysinfo.GPU_Count = iAdapterNum;
		for (size_t i = 0; i < vAdapters.size(); i++)
		{
			GpuInfo gpuinfo;
			DXGI_ADAPTER_DESC adapterDesc;
			vAdapters[i]->GetDesc(&adapterDesc);
			gpuinfo.description = std::wstring(adapterDesc.Description);
			gpuinfo.DedicatedVideoMemory_M = adapterDesc.DedicatedVideoMemory / 1024 / 1024;
			gpuinfo.SharedSystemMemory_M = adapterDesc.SharedSystemMemory / 1024 / 1024;
			gpuinfo.DedicatedSystemMemory_M = adapterDesc.DedicatedSystemMemory / 1024 / 1024;
			gpuinfo.VendorId = adapterDesc.VendorId;
			gpuinfo.DeviceId = adapterDesc.DeviceId;
			gpuinfo.SubSysId = adapterDesc.SubSysId;
			gpuinfo.Revision = adapterDesc.Revision;
			gpuinfo.AdapterLuid = adapterDesc.AdapterLuid;

			IDXGIOutput* pOutput;
			std::vector<IDXGIOutput*> vOutputs;
			int iOutputNum = 0;
			while (vAdapters[i]->EnumOutputs(iOutputNum, &pOutput) != DXGI_ERROR_NOT_FOUND)
			{
				vOutputs.push_back(pOutput);
				iOutputNum++;
			}
			gpuinfo.output_device_count = iOutputNum;


			for (size_t n = 0; n < vOutputs.size(); n++)
			{
				// 获取显示设备信息  
				DXGI_OUTPUT_DESC outputDesc;
				vOutputs[n]->GetDesc(&outputDesc);

				// 获取设备支持  
				UINT uModeNum = 0;
				DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
				UINT flags = DXGI_ENUM_MODES_INTERLACED;

				vOutputs[n]->GetDisplayModeList(format, flags, &uModeNum, 0);
				DXGI_MODE_DESC* pModeDescs = new DXGI_MODE_DESC[uModeNum];
				vOutputs[n]->GetDisplayModeList(format, flags, &uModeNum, pModeDescs);

				std::pair<int, int> p;
				p.first = outputDesc.DesktopCoordinates.right - outputDesc.DesktopCoordinates.left;
				p.second = outputDesc.DesktopCoordinates.bottom - outputDesc.DesktopCoordinates.top;
				gpuinfo.avil_ratio.push_back(p);
			}
			vOutputs.clear();
			m_sysinfo.GPU_infos.push_back(gpuinfo);
		}
	}
}

std::vector<ProcessObj> SysInfoColl::GetProcessDetils(bool setdebugflag)
{
	std::vector<ProcessObj> result;
	m_sysinfo.getprocessinfowarning = GetProcessInfoWarning::GPIW_NO_ERROR;
	HANDLE hProcessSnap;
	HANDLE hToken;
	PROCESSENTRY32 stcPe32 = { 0 };
	BOOLEAN isSucess = FALSE;
	stcPe32.dwSize = sizeof(PROCESSENTRY32);
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		m_sysinfo.getprocessinfowarning = m_sysinfo.getprocessinfowarning | GetProcessInfoWarning::FAILED_TO_CREATE_SNAPSHOT;
		return result;
	}
	if (!Process32First(hProcessSnap, &stcPe32)) {
		m_sysinfo.getprocessinfowarning = m_sysinfo.getprocessinfowarning | GetProcessInfoWarning::FAILED_TO_START_FIRSTFUNC;
		CloseHandle(hProcessSnap);
		return result;
	}
	if (setdebugflag)
	{
		if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken))
		{
			if (GetLastError() == ERROR_NO_TOKEN)
			{
				if (ImpersonateSelf(SecurityImpersonation)) {
					if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken))
					{
						m_sysinfo.getprocessinfowarning = m_sysinfo.getprocessinfowarning | GetProcessInfoWarning::FAILED_TO_OPENTHREADTOKEN;
					}
				}
				else {
					m_sysinfo.getprocessinfowarning = m_sysinfo.getprocessinfowarning | GetProcessInfoWarning::FAILED_TO_IMPERSONATESELF;
				}
			}
		}
		if (m_sysinfo.getprocessinfowarning == GetProcessInfoWarning::GPIW_NO_ERROR)
		{
			SetPrivilege(hToken, SE_DEBUG_NAME, TRUE);
		}
	}
	do {
		ProcessObj obj;
		obj.parentProcessId = stcPe32.th32ParentProcessID;
		obj.processId = stcPe32.th32ProcessID;
		wmemcpy(obj.exeFile, stcPe32.szExeFile, MAX_PATH);
		obj.getprocessdetilswarning = GetProcessDetilsWarning::GPDW_NO_ERROR;
		HANDLE hProcess; 
		PROCESS_MEMORY_COUNTERS pmc;
		hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, stcPe32.th32ProcessID);
		if (hProcess) {
			if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
			{
				obj.parseFrom(pmc);
			}
			else {
				obj.getprocessdetilswarning = obj.getprocessdetilswarning | GetProcessDetilsWarning::FAILED_QUERY_MEMORY_INFO;
			}

			if (GetProcessTimes(hProcess, &obj.lpCreationTime, &obj.lpExitTime, &obj.lpKernelTime, &obj.lpUserTime))
			{
				obj.cpuUsageTime = (static_cast<double>(obj.FileTimeToInt64(obj.lpKernelTime)) + obj.FileTimeToInt64(obj.lpUserTime)) / (m_sysinfo.cpuinfo.dwNumberOfProcessors * 1.0);
			}
			else {
				obj.getprocessdetilswarning = obj.getprocessdetilswarning | GetProcessDetilsWarning::FAILED_QUERY_TIMES_INFO;
			}
			CloseHandle(hProcess);
		}
		else {
			obj.getprocessdetilswarning = obj.getprocessdetilswarning | GetProcessDetilsWarning::FAILED_OPEN_PROCESS;
		}
		result.push_back(obj);
	} while (Process32Next(hProcessSnap, &stcPe32));
	CloseHandle(hProcessSnap);
	if (setdebugflag && hToken)
	{
		SetPrivilege(hToken, SE_DEBUG_NAME, FALSE);
	}
	return result;
}

std::vector<ServicesObj> SysInfoColl::GetSystemServicesList()
{
	SC_HANDLE schSCService;
	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	void* buf_for_status = NULL, * buf_for_config = NULL;
	DWORD bufSize = 0;
	DWORD moreBytesNeeded, serviceCount;
	ENUM_SERVICE_STATUS_PROCESS* servicesStatus;
	QUERY_SERVICE_CONFIG* servicesConfig;

	for (;;)
	{
		if (EnumServicesStatusEx(schSCManager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL,
			(LPBYTE)buf_for_status, bufSize, &moreBytesNeeded, &serviceCount, NULL, NULL))
		{
			servicesStatus = (ENUM_SERVICE_STATUS_PROCESS*)buf_for_status;
			break;
		}

		int err = GetLastError();
		if (ERROR_MORE_DATA != err)
		{
			free(buf_for_status);
			return std::vector<ServicesObj>();
		}
		bufSize += moreBytesNeeded;
		buf_for_status = new LPSTR[bufSize]();
		moreBytesNeeded = 0;
	}

	std::vector<ServicesObj> servList(serviceCount);
	std::wstring tmpStr;

	for (size_t i = 0; i < serviceCount; i++)
	{
		servList[i].m_name = servicesStatus[i].lpServiceName;
		servList[i].m_pid = servicesStatus[i].ServiceStatusProcess.dwProcessId;
		servList[i].m_dispName = servicesStatus[i].lpDisplayName;
		servList[i].m_state = servicesStatus[i].ServiceStatusProcess.dwCurrentState;

		buf_for_config = new QUERY_SERVICE_CONFIG[4096]();

		schSCService = OpenService(schSCManager, servicesStatus[i].lpServiceName, SERVICE_ALL_ACCESS);
		QueryServiceConfig(schSCService, (LPQUERY_SERVICE_CONFIGW)buf_for_config, 4096, &moreBytesNeeded);
		servicesConfig = (LPQUERY_SERVICE_CONFIGW)buf_for_config;

		if (servicesConfig->lpBinaryPathName != nullptr)
		{
			tmpStr = servicesConfig->lpBinaryPathName;

			int flag = tmpStr.find(L"-k");
			if (flag > 0)
			{
				servList[i].m_path.insert(servList[i].m_path.begin(), tmpStr.begin(), tmpStr.begin() + flag);
				servList[i].m_group.insert(servList[i].m_group.begin(), tmpStr.begin() + flag + 2, tmpStr.end());
			}
			else servList[i].m_path.insert(servList[i].m_path.begin(), tmpStr.begin(), tmpStr.end());


		}
		CloseServiceHandle(schSCService);
	}
	return servList;
}

std::vector<GpuInfo_Nvml> SysInfoColl::GetGpuInfoByNvml()
{
	std::vector<GpuInfo_Nvml> result;
	m_sysinfo.getgpuinfowarningfromnv = GetGPUInfoWarning::GGIW_NO_ERROR;
	Nvml_Sic::nvmlReturn_enum res;
	unsigned int device_count;
	HINSTANCE nvml_dll = NULL;
	nvml_dll = LoadLibrary(L"nvml.dll");
	if (!nvml_dll) {
		m_sysinfo.getgpuinfowarningfromnv = m_sysinfo.getgpuinfowarningfromnv | GetGPUInfoWarning::FAILED_LOAD_NVML_DLL;
		return result;
	}
	Nvml_Sic::nvmlInit_nvml nvmlInitFunc = (Nvml_Sic::nvmlInit_nvml)GetProcAddress(nvml_dll, "nvmlInit");
	Nvml_Sic::nvmlShutdown_nvml nvmlShutdownFunc = (Nvml_Sic::nvmlShutdown_nvml)GetProcAddress(nvml_dll, "nvmlShutdown");
	Nvml_Sic::nvmlErrorString_nvml nvmlErrorStringFunc = (Nvml_Sic::nvmlErrorString_nvml)GetProcAddress(nvml_dll, "nvmlErrorString");
	Nvml_Sic::nvmlDeviceGetCount_nvml nvmlDeviceGetCountFunc = (Nvml_Sic::nvmlDeviceGetCount_nvml)GetProcAddress(nvml_dll, "nvmlDeviceGetCount");
	Nvml_Sic::nvmlDeviceGetHandleByIndex_nvml nvmlDeviceGetHandleByIndexFunc = (Nvml_Sic::nvmlDeviceGetHandleByIndex_nvml)GetProcAddress(nvml_dll, "nvmlDeviceGetHandleByIndex");
	Nvml_Sic::nvmlDeviceGetName_nvml nvmlDeviceGetNameFunc = (Nvml_Sic::nvmlDeviceGetName_nvml)GetProcAddress(nvml_dll, "nvmlDeviceGetName");
	Nvml_Sic::nvmlDeviceGetUtilizationRates_nvml nvmlDeviceGetUtilizationRatesFunc = (Nvml_Sic::nvmlDeviceGetUtilizationRates_nvml)GetProcAddress(nvml_dll, "nvmlDeviceGetUtilizationRates");
	Nvml_Sic::nvmlDeviceGetMemoryInfo_nvml nvmlDeviceGetMemoryInfoFunc = (Nvml_Sic::nvmlDeviceGetMemoryInfo_nvml)GetProcAddress(nvml_dll, "nvmlDeviceGetMemoryInfo");
	Nvml_Sic::nvmlDeviceGetComputeRunningProcesses_nvml nvmlDeviceGetComputeRunningProcessesFunc = (Nvml_Sic::nvmlDeviceGetComputeRunningProcesses_nvml)GetProcAddress(nvml_dll, "nvmlDeviceGetComputeRunningProcesses");
	if (!nvmlInitFunc) {
		m_sysinfo.getgpuinfowarningfromnv = m_sysinfo.getgpuinfowarningfromnv | GetGPUInfoWarning::FAILED_LOAD_NVMLINITFUNC_;
		return result;
	}
	if (!nvmlShutdownFunc) {
		m_sysinfo.getgpuinfowarningfromnv = m_sysinfo.getgpuinfowarningfromnv | GetGPUInfoWarning::FAILED_LOAD_NVMLSHUTDOWNFUNC_;
		return result;
	}
	if (!nvmlDeviceGetNameFunc) {
		m_sysinfo.getgpuinfowarningfromnv = m_sysinfo.getgpuinfowarningfromnv | GetGPUInfoWarning::FAILED_LOAD_NVMLDEVICEGETNAMEFUNC_;
	}
	if (!nvmlDeviceGetUtilizationRatesFunc) {
		m_sysinfo.getgpuinfowarningfromnv = m_sysinfo.getgpuinfowarningfromnv | GetGPUInfoWarning::FAILED_LOAD_NVMLDEVICEGETUTILIZATIONTATESFUNC_;
	}
	if (!nvmlDeviceGetMemoryInfoFunc) {
		m_sysinfo.getgpuinfowarningfromnv = m_sysinfo.getgpuinfowarningfromnv | GetGPUInfoWarning::FAILED_LOAD_NVMLDEVICEGETMEMORYINFOFUNC_;
	}
	if (!nvmlDeviceGetComputeRunningProcessesFunc) {
		m_sysinfo.getgpuinfowarningfromnv = m_sysinfo.getgpuinfowarningfromnv | GetGPUInfoWarning::FAILED_LOAD_NVMLDEVICEGETCOMPUTERUNNINGPROCESSESFUNC_;
	}
	res = nvmlInitFunc();
	if (res != Nvml_Sic::NVML_SUCCESS)
	{
		m_sysinfo.getgpuinfowarningfromnv = m_sysinfo.getgpuinfowarningfromnv | GetGPUInfoWarning::FAILED_NVML_INIT;
		m_sysinfo.getgpuinfonvlasterror = res;
		goto break_get_gpuinfo_by_nvml;
	}
	if (!nvmlDeviceGetCountFunc) {
		m_sysinfo.getgpuinfowarningfromnv = m_sysinfo.getgpuinfowarningfromnv | GetGPUInfoWarning::FAILED_LOAD_NVMLGETCOUNTFUNC_;
		m_sysinfo.getgpuinfonvlasterror = res;
		goto break_get_gpuinfo_by_nvml;
	}
	res = nvmlDeviceGetCountFunc(&device_count);
	if (res != Nvml_Sic::NVML_SUCCESS)
	{
		m_sysinfo.getgpuinfowarningfromnv = m_sysinfo.getgpuinfowarningfromnv | GetGPUInfoWarning::FAILED_GET_DEVICE_GPU_COUNT;
		m_sysinfo.getgpuinfonvlasterror = res;
		goto break_get_gpuinfo_by_nvml;
	}
	if (!nvmlDeviceGetHandleByIndexFunc) {
		m_sysinfo.getgpuinfowarningfromnv = m_sysinfo.getgpuinfowarningfromnv | GetGPUInfoWarning::FAILED_LOAD_NVMLGETDEVICEHANDLEBUINDEXFUNC_;
		m_sysinfo.getgpuinfonvlasterror = res;
		goto break_get_gpuinfo_by_nvml;
	}
	for (int i = 0; i < device_count; i++)
	{
		Nvml_Sic::nvmlDevice_t device;
		res = nvmlDeviceGetHandleByIndexFunc(i, &device);
		m_sysinfo.getgpuinfolooperrors.insert(std::make_pair(i, std::pair<int, std::vector<std::string>>(0, std::vector<std::string>())));
		if (res != Nvml_Sic::NVML_SUCCESS)
		{
			m_sysinfo.getgpuinfolooperrors[i].first = m_sysinfo.getgpuinfolooperrors[i].first | GetGPUInfoWarning::FAILED_GET_DEVICE_HANDLE;
			m_sysinfo.getgpuinfolooperrors[i].second.push_back(std::string(nvmlErrorStringFunc(res)));
			continue;
		}
		GpuInfo_Nvml ginInfo;
		if (nvmlDeviceGetNameFunc) {
			res = nvmlDeviceGetNameFunc(device, ginInfo.name, NVML_DEVICE_NAME_BUFFER_SIZE);
			if (res != Nvml_Sic::NVML_SUCCESS)
			{
				m_sysinfo.getgpuinfolooperrors[i].first = m_sysinfo.getgpuinfolooperrors[i].first | GetGPUInfoWarning::FAILED_GET_DEVICE_NAME;
				m_sysinfo.getgpuinfolooperrors[i].second.push_back(std::string(nvmlErrorStringFunc(res)));
			}
		}
		
		Nvml_Sic::nvmlUtilization_t utilization{ 0 };
		if (nvmlDeviceGetUtilizationRatesFunc) {
			res = nvmlDeviceGetUtilizationRatesFunc(device, &utilization);
			if (res != Nvml_Sic::NVML_SUCCESS)
			{
				m_sysinfo.getgpuinfolooperrors[i].first = m_sysinfo.getgpuinfolooperrors[i].first | GetGPUInfoWarning::FAILED_GET_DEVICE_UTILIZATION_RATES;
				m_sysinfo.getgpuinfolooperrors[i].second.push_back(std::string(nvmlErrorStringFunc(res)));
			}
			else {
				ginInfo.util_gpu = utilization.gpu;
				ginInfo.util_memory = utilization.memory;
			}
		}
		Nvml_Sic::nvmlMemory_t memory{ 0 };
		if (nvmlDeviceGetMemoryInfoFunc) {
			res = nvmlDeviceGetMemoryInfoFunc(device, &memory);
			if (res != Nvml_Sic::NVML_SUCCESS)
			{
				m_sysinfo.getgpuinfolooperrors[i].first = m_sysinfo.getgpuinfolooperrors[i].first | GetGPUInfoWarning::FAILED_GET_DEVICE_MEMORY_INFO;
				m_sysinfo.getgpuinfolooperrors[i].second.push_back(std::string(nvmlErrorStringFunc(res)));
			}
			else {
				ginInfo.used = memory.used;
				ginInfo.free = memory.free;
				ginInfo.total = memory.total;
			}
		}
		unsigned int infoCnt;
		Nvml_Sic::nvmlProcessInfo_t* nvProcInfos{};
		if (nvmlDeviceGetComputeRunningProcessesFunc) {
			res = nvmlDeviceGetComputeRunningProcessesFunc(device, &infoCnt, nvProcInfos);
			if (res != Nvml_Sic::NVML_SUCCESS)
			{
				m_sysinfo.getgpuinfolooperrors[i].first = m_sysinfo.getgpuinfolooperrors[i].first | GetGPUInfoWarning::FAILED_GET_DEVICE_QUERY_RUNNINGPROCESS;
				m_sysinfo.getgpuinfolooperrors[i].second.push_back(std::string(nvmlErrorStringFunc(res)));
			}
			else {
				for (int j = 0; j < infoCnt; j++)
				{
					Nvml_Sic::nvmlProcessInfo_t info = nvProcInfos[j];
					ginInfo.proc_mem.insert(std::make_pair(info.pid, info.usedGpuMemory / DIV / DIV));
				}
			}
		}
		result.push_back(ginInfo);
	}

break_get_gpuinfo_by_nvml:
	nvmlShutdownFunc();
	FreeLibrary(nvml_dll);
	return result;
}


int SysInfoColl::GetSystemRasConnections()
{
	DWORD dwCb = 0;
	DWORD dwRet = ERROR_SUCCESS;
	DWORD dwConnections = 0;
	LPRASCONN lpRasConn = NULL;

	dwRet = RasEnumConnections(lpRasConn, &dwCb, &dwConnections);
	if (dwRet == ERROR_BUFFER_TOO_SMALL) {
		lpRasConn = (LPRASCONN)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwCb);
		if (lpRasConn == NULL) {
			return dwConnections;
		}
		lpRasConn[0].dwSize = sizeof(RASCONN);

		dwRet = RasEnumConnections(lpRasConn, &dwCb, &dwConnections);

		if (ERROR_SUCCESS == dwRet) {
			for (DWORD i = 0; i < dwConnections; i++) {
				wprintf(L"%s\n", lpRasConn[i].szEntryName);
			}
		}
		HeapFree(GetProcessHeap(), 0, lpRasConn);
		lpRasConn = NULL;
		return dwConnections;
	}
}

BOOL SysInfoColl::GetWinetProxy(LPSTR lpszProxy, UINT nProxyLen)
{
	unsigned long        nSize = 4096;
	char                 szBuf[4096] = { 0 };
	INTERNET_PROXY_INFO* pProxyInfo = (INTERNET_PROXY_INFO*)szBuf;
	if (!InternetQueryOption(NULL, INTERNET_OPTION_PROXY, pProxyInfo, &nSize))
	{
		return FALSE;
	}
	if (pProxyInfo->dwAccessType == INTERNET_OPEN_TYPE_DIRECT)
	{
		return FALSE;
	}

	LPCSTR lpszProxyList = (LPCSTR)(pProxyInfo + 1);
	int nLen = strlen(lpszProxyList);
	if (lpszProxy)
	{

		nProxyLen = min(nLen, nProxyLen - 1);
		strncpy_s(lpszProxy, nProxyLen + 1, lpszProxyList, nLen);
		lpszProxy[nProxyLen] = 0;
	}
	return TRUE;
}

std::string SysInfoColl::GetDxVersion()
{
	HKEY  hKeyResult = NULL;
	DWORD dwSize = 0;
	DWORD dwDataType = 0;
	std::string strValue("");
	if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\DirectX", 0, KEY_QUERY_VALUE, &hKeyResult))
	{
		::RegQueryValueEx(hKeyResult, L"Version", 0, &dwDataType, NULL, &dwSize);
		switch (dwDataType)
		{
		case REG_MULTI_SZ:
		{
			//分配内存大小
			BYTE* lpValue = new BYTE[dwSize];
			//获取注册表中指定的键所对应的值
			LONG lRet = ::RegQueryValueEx(hKeyResult, L"Version", 0, &dwDataType, lpValue, &dwSize);
			delete[] lpValue;
			break;
		}
		case REG_SZ:
		{
			//分配内存大小
			wchar_t* lpValue = new wchar_t[dwSize];
			memset(lpValue, 0, dwSize * sizeof(wchar_t));
			//获取注册表中指定的键所对应的值
			if (ERROR_SUCCESS == ::RegQueryValueEx(hKeyResult, L"Version", 0, &dwDataType, (LPBYTE)lpValue, &dwSize))
			{
				std::wstring wstrValue(lpValue);
				strValue = Data_Comm::ws2s(wstrValue);
			}
			delete[] lpValue;
			break;
		}
		default:
			break;
		}
	}
	::RegCloseKey(hKeyResult);
	return strValue;
}

void SysInfoColl::GetSystemFoudationInfo()
{
	//获取系统基本参数
	{
		m_sysinfo.osinfo.getsysteminfowarning = GetSystemInfoWarning::GSIW_NO_ERROR;
		SYSTEM_INFO system_info;
		memset(&system_info, 0, sizeof(SYSTEM_INFO));
		GetSystemInfo(&system_info);
		m_sysinfo.cpuinfo.dwNumberOfProcessors = system_info.dwNumberOfProcessors;
		OSVERSIONINFOEX os;
		os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if (GetVersionEx((OSVERSIONINFO*)&os))
		{
			m_sysinfo.osinfo.majorVersion = os.dwMajorVersion;
			m_sysinfo.osinfo.minorVersion = os.dwMinorVersion;
			m_sysinfo.osinfo.buildNumber = os.dwBuildNumber;
			m_sysinfo.osinfo.platformId = os.dwPlatformId;
			m_sysinfo.osinfo.serciveMajor = os.wServicePackMajor;
			m_sysinfo.osinfo.serciveMinor = os.wServicePackMinor;
		}
		else
		{
			m_sysinfo.osinfo.getsysteminfowarning = GetSystemInfoWarning::GET_VERSION_ERR;
		}
	}
	//获取CPU信息
	{
		m_sysinfo.getcpuinfowarning = GetCPUInfoWarning::GCIW_NO_ERROR;
		std::wstring strPath = L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0";
		CRegKey regkey;
		LONG lResult;
		DWORD dwSize = 50;
		wmemset(m_sysinfo.cpuinfo.cpuname, 0, 50);
		lResult = regkey.Open(HKEY_LOCAL_MACHINE, strPath.c_str(), KEY_ALL_ACCESS);
		if (lResult != ERROR_SUCCESS)
		{
			m_sysinfo.getcpuinfowarning = m_sysinfo.getcpuinfowarning | GetCPUInfoWarning::FAILED_TO_OPEN_REGEDIT;
			goto start_check_cpu_core;
		}

		if (ERROR_SUCCESS != regkey.QueryStringValue(_T("ProcessorNameString"), m_sysinfo.cpuinfo.cpuname, &dwSize))
		{
			m_sysinfo.getcpuinfowarning = m_sysinfo.getcpuinfowarning | GetCPUInfoWarning::FAILED_TO_QUERY_PROCESSNAMESTRING;
		}

		//查询CPU主频  
		if (ERROR_SUCCESS != regkey.QueryDWORDValue(_T("~MHz"), m_sysinfo.cpuinfo.mhz))
		{
			m_sysinfo.getcpuinfowarning = m_sysinfo.getcpuinfowarning | GetCPUInfoWarning::FAILED_TO_QUERY_MHZ;
		}
		regkey.Close();

	start_check_cpu_core:
		//获取CPU核心数目  
		SYSTEM_INFO si;
		memset(&si, 0, sizeof(SYSTEM_INFO));
		GetSystemInfo(&si);
		DWORD dwNum = si.dwNumberOfProcessors;
		switch (si.dwProcessorType)
		{
		case PROCESSOR_INTEL_386:
			m_sysinfo.cpuinfo.chProcessorType = "Intel 386 processor";
			break;
		case PROCESSOR_INTEL_486:
			m_sysinfo.cpuinfo.chProcessorType = "Intel 486 Processor";
			break;
		case PROCESSOR_INTEL_PENTIUM:
			m_sysinfo.cpuinfo.chProcessorType = "Intel Pentium Processor";
			break;
		case PROCESSOR_INTEL_IA64:
			m_sysinfo.cpuinfo.chProcessorType = "Intel IA64 Processor";
			break;
		case PROCESSOR_AMD_X8664:
			m_sysinfo.cpuinfo.chProcessorType = "AMD X8664 Processor";
			break;
		default:
			m_sysinfo.cpuinfo.chProcessorType = "Unknown";
			break;
		}

		int cpuInfo[4] = { -1 };

		__cpuid(cpuInfo, 0x80000002);
		memcpy(m_sysinfo.cpuinfo.cpu_manufacture, cpuInfo, sizeof(cpuInfo));

		__cpuid(cpuInfo, 0x80000003);
		memcpy(m_sysinfo.cpuinfo.cpu_type, cpuInfo, sizeof(cpuInfo));

		__cpuid(cpuInfo, 0x80000004);
		memcpy(m_sysinfo.cpuinfo.cpu_freq, cpuInfo, sizeof(cpuInfo));
	}
}

std::vector<DeviceObj> SysInfoColl::GetAllDevices()
{
	std::vector<DeviceObj> result;
	LPGUID lpGuid = (LPGUID)&GUID_CLASS_USB_HUB;//
	int INTERFACE_DETAIL_SIZE = 1024;
	int nCount = 0;
	HDEVINFO info = SetupDiGetClassDevs(lpGuid, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);

	//enumerate device information  
	DWORD required_size = 0;
	int i;
	SP_DEVINFO_DATA DeviceInfoData = { sizeof(DeviceInfoData) };

	DWORD DataT;
	DWORD buffersize = 1024;
	DWORD req_bufsize = 0;

	for (i = 0; SetupDiEnumDeviceInfo(info, i, &DeviceInfoData); i++)
	{
		DeviceObj obj;
		if (!SetupDiGetDeviceInstanceId(info, &DeviceInfoData, (PTSTR)obj.instanceId, buffersize, &req_bufsize))
			continue;
		if (!SetupDiGetDeviceRegistryPropertyW(info, &DeviceInfoData, SPDRP_DEVICEDESC, &DataT, (LPBYTE)obj.deviceDesc, buffersize, &req_bufsize))
			continue;
		if (!SetupDiGetDeviceRegistryPropertyW(info, &DeviceInfoData, SPDRP_FRIENDLYNAME, &DataT, (LPBYTE)obj.friendName, buffersize, &req_bufsize))
		{
			//continue;
		}
		if (!SetupDiGetDeviceRegistryPropertyW(info, &DeviceInfoData, SPDRP_MFG, &DataT, (LPBYTE)obj.producer, buffersize, &req_bufsize))
		{
			//continue;
		}
		if (!SetupDiGetDeviceRegistryPropertyW(info, &DeviceInfoData, SPDRP_LOCATION_PATHS, &DataT, (LPBYTE)obj.locationPaths, buffersize, &req_bufsize))
		{
			//continue;
		}
		if (!SetupDiGetDeviceRegistryPropertyW(info, &DeviceInfoData, SPDRP_LOCATION_INFORMATION, &DataT, (LPBYTE)obj.locationInfo, buffersize, &req_bufsize))
		{
			//continue;
		}

		result.push_back(obj);
	}

	SetupDiDestroyDeviceInfoList(info);
	return result;
}


BOOL SysInfoColl::SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
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

void SysInfoColl::GetTimestamp(const DWORD Time, WCHAR DisplayString[])
{
	ULONGLONG ullTimeStamp = 0;
	ULONGLONG SecsTo1970 = 116444736000000000;
	SYSTEMTIME st;
	FILETIME ft, ftLocal;

	ullTimeStamp = Int32x32To64(Time, 10000000) + SecsTo1970;
	ft.dwHighDateTime = (DWORD)((ullTimeStamp >> 32) & 0xFFFFFFFF);
	ft.dwLowDateTime = (DWORD)(ullTimeStamp & 0xFFFFFFFF);

	FileTimeToLocalFileTime(&ft, &ftLocal);
	FileTimeToSystemTime(&ftLocal, &st);
	StringCchPrintf(DisplayString, MAX_TIMESTAMP_LEN, L"%d-%d-%d %.2d:%.2d:%.2d",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}

DT::SystemInfos* SysInfoColl::GetSystemInfosStruct()
{
	return &m_sysinfo;
}