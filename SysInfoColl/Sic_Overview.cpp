#include "Sic_Overview.h"

Sic_Overview::Sic_Overview()
{
	if (!m_overview) {
		m_overview = new Overview;
	}
	m_sysInfo->m_OverView = m_overview;
}

Sic_Overview::~Sic_Overview()
{
	if (m_overview) {
		delete m_overview;
		m_overview = nullptr;
	}
	m_sysInfo->m_OverView = nullptr;
}

void Sic_Overview::Execute(void)
{
	GetSystemOverView();
	GetVersionInfo();
	GetCpuInfo();
}

std::string Sic_Overview::GetErrors(void)
{
	return std::string();
}

void Sic_Overview::GetSystemOverView(void)
{
	m_overview->m_runInfo->Up_time = GetTickCount64();
	DWORD  bufCharCount = INFO_BUFFER_SIZE;
	GetUserName(m_overview->m_runInfo->username, &bufCharCount);
	PULONGLONG memoryInstalled;
	ZeroMemory(memoryInstalled, sizeof(PULONGLONG));
	if (GetPhysicallyInstalledSystemMemory(memoryInstalled))
	{
		m_overview->m_runInfo->physicallyMemoryInstalled = *memoryInstalled / DIV / DIV;
	}
	else {
		m_overview->Status |= OverViewStatus::OVS_CANNOT_GET_PHYSICALLYMEMORYINSTALLED;
	}
}

void Sic_Overview::GetVersionInfo(void)
{
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (GetVersionEx((OSVERSIONINFO*)&os))
	{
		m_overview->m_osInfo->majorVersion = os.dwMajorVersion;
		m_overview->m_osInfo->minorVersion = os.dwMinorVersion;
		m_overview->m_osInfo->buildNumber = os.dwBuildNumber;
		m_overview->m_osInfo->platformId = os.dwPlatformId;
		m_overview->m_osInfo->serciveMajor = os.wServicePackMajor;
		m_overview->m_osInfo->serciveMinor = os.wServicePackMinor;
	}
	else {
		m_overview->Status |= OverViewStatus::OVS_CANNOT_GET_VERSION;
	}
}

void Sic_Overview::GetCpuInfo(void)
{
#ifdef WIN32
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	m_overview->m_cpuInfo->CPUCoreCount = sysInfo.dwNumberOfProcessors;
	BOOL done = FALSE; 
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
	DWORD returnLength = 0;
	LPFN_GLPI glpi;
	glpi = (LPFN_GLPI)GetProcAddress(GetModuleHandle(TEXT(KERNEL32_DLL)),	"GetLogicalProcessorInformation");
	if (!glpi) {
		m_overview->Status |= OverViewStatus::OVS_CANNOT_GET_FUNC_GETLOGICALPROCESSORINFORM;
		return;
	}
	while (!done)
	{
		DWORD rc = glpi(buffer, &returnLength);
		if (!rc) 
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				if (buffer)
					free(buffer);
				buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(returnLength);
				if (!buffer)
				{
					m_overview->Status |= OverViewStatus::OVS_CANNOT_ALLOCATION_MEM;
					return;
				}
			}
			else {
				m_overview->m_cpuInfo->LastError = GetLastError();
				return;
			}
		}
		else
		{
			done = TRUE;
		}
	}
	ptr = buffer; 
	DWORD byteOffset = 0;
	PCACHE_DESCRIPTOR Cache;
	while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength)
	{
		switch (ptr->Relationship)
		{
		case RelationNumaNode:
			m_overview->m_cpuInfo->numaNodeCount++;
			break;
		case RelationProcessorCore:
			m_overview->m_cpuInfo->processorCoreCount++;
			break;
		case RelationCache:
			Cache = &ptr->Cache;
			if (Cache->Level == 1) {
				m_overview->m_cpuInfo->processorL1CacheCount++;
			}
			else if (Cache->Level == 2) {
				m_overview->m_cpuInfo->processorL2CacheCount++;
			}
			else if (Cache->Level == 3) {
				m_overview->m_cpuInfo->processorL3CacheCount++;
			}
			break;
		case RelationProcessorPackage:
			m_overview->m_cpuInfo->processorPackageCount++;
			break;
		}
		byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
		ptr++;
	}
	free(buffer);
#else
	m_overview->m_cpuInfo->CPU_Core_Count = sysconf(_SC_NPROCESSORS_CONF);
	m_overview->m_cpuInfo->enableCoreCount = sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

Sic_Overview* Sic_Overview::getInstance()
{
	m_mutex_overview->lock();
	if (instance == nullptr) {
		instance = new Sic_Overview;
	}
	m_mutex_overview->unlock();
	return instance;
}