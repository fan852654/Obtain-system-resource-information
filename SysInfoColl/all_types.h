#pragma once
#ifndef ALL_TYPES_H

#define ALL_TYPES_H

#include "winsock2.h"
#include <intrin.h>
#include <Windows.h>
#include <iostream>
#include <atlbase.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <SetupAPI.h>
#include <usbiodef.h>
#include <Usbioctl.h>
#include <devguid.h>
#include <winioctl.h>
#include <INITGUID.h>
#include <Dbt.h>
#include "ras.h"
#include "raserror.h"
#include "wininet.h"
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"psapi.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "rasapi32.lib")
#pragma comment(lib, "Wininet.lib")
#include <vector>
#include <map>
#include <set>
#include <tchar.h>
#include <stdio.h>
#include <nvml.h>
#include <cuda_runtime_api.h>
#include <DXGI.h>
#pragma comment(lib,"DXGI.lib")
#include <strsafe.h>

#define PROVIDER_NAME           L"MyEventProvider"
#define RESOURCE_DLL            L"<path>\\Provider.dll"
#define MAX_TIMESTAMP_LEN       23 + 1   // mm/dd/yyyy hh:mm:ss.mmm
#define MAX_RECORD_BUFFER_SIZE  0x10000  // 64K
#define INFO_BUFFER_SIZE 32767
#define DIV 1024
#define WIDTH 7
DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE,
	0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED);
#define   GUID_CLASS_USB_DEVICE                   GUID_DEVINTERFACE_USB_DEVICE  
DEFINE_GUID(GUID_DEVINTERFACE_USB_HUB,
	0xf18a0e88, 0xc30c, 0x11d0, 0x88, 0x15, 0x00, 0xa0, 0xc9, 0x06, 0xbe, 0xd8);

#define		GUID_CLASS_USB_HUB GUID_DEVINTERFACE_USB_HUB
namespace DT
{
	enum MonitorColType
	{
		USE_DC,
		USE_SYS
	};
	enum GetSystemInfoWarning
	{
		GSIW_NO_ERROR,
		GET_VERSION_ERR
	};
	enum GetCPUInfoWarning
	{
		GCIW_NO_ERROR,
		FAILED_TO_OPEN_REGEDIT,
		FAILED_TO_QUERY_PROCESSNAMESTRING,
		FAILED_TO_QUERY_MHZ
	};
	enum GetGPUInfoWarning
	{
		GGIW_NO_ERROR,
		CREATE_DXGIFACTORY_FAILED,
		FAILED_NVML_INIT,
		FAILED_GET_DEVICE_GPU_COUNT,
		FAILED_GET_DEVICE_HANDLE,
		FAILED_GET_DEVICE_NAME,
		FAILED_GET_DEVICE_UTILIZATION_RATES,
		FAILED_GET_DEVICE_MEMORY_INFO,
		FAILED_GET_DEVICE_QUERY_RUNNINGPROCESS
	};
	enum GetProcessInfoWarning
	{
		GPIW_NO_ERROR,
		FAILED_TO_CREATE_SNAPSHOT,
		FAILED_TO_START_FIRSTFUNC,
		FAILED_TO_OPENTHREADTOKEN,
		FAILED_TO_IMPERSONATESELF
	};
	enum GetProcessDetilsWarning
	{
		GPDW_NO_ERROR,
		FAILED_OPEN_PROCESS,
		FAILED_QUERY_MEMORY_INFO,
		FAILED_QUERY_TIMES_INFO
	};
	enum GetEventLogWarning
	{
		GELW_NO_ERROR,
		FAILED_NO_ENOUGH_MEMORY,
		FAILED_REALLOC_MEMORY,
		FAILED_OPEN_EVENT_LOG
	};
	struct ServicesObj
	{
		WCHAR* m_name;
		unsigned int m_pid = 0;
		std::wstring m_dispName;
		unsigned int m_state = 0;
		std::wstring  m_group;
		std::wstring  m_path;
	};
	struct DeviceObj
	{
		wchar_t deviceDesc[1024];
		wchar_t instanceId[1024];
		wchar_t locationPaths[1024];
		wchar_t locationInfo[1024];
		wchar_t friendName[1024];
		wchar_t producer[1024];
	};
	struct ProcessObj
	{
		int getprocessdetilswarning;
		DWORD processId;
		DWORD parentProcessId;
		WCHAR exeFile[MAX_PATH];

		int PeakWorkingSetSize;
		int WorkingSetSize;
		int QuotaPeakPagedPoolUsage;
		int QuotaPagedPoolUsage;
		int QuotaPeakNonPagedPoolUsage;
		int QuotaNonPagedPoolUsage;
		int PagefileUsage;
		int PeakPagefileUsage;

		LPFILETIME lpCreationTime;
		LPFILETIME lpExitTime;
		LPFILETIME lpKernelTime;
		LPFILETIME lpUserTime;

		double cpuUsageTime;
		double cpuUsage;

		void parseFrom(const PROCESS_MEMORY_COUNTERS& pmc) {
			PeakWorkingSetSize = pmc.PeakWorkingSetSize / DIV / DIV;
			WorkingSetSize = pmc.WorkingSetSize / DIV / DIV;
			QuotaPeakPagedPoolUsage = pmc.QuotaPeakPagedPoolUsage / DIV / DIV;
			QuotaPagedPoolUsage = pmc.QuotaPagedPoolUsage / DIV / DIV;
			QuotaPeakNonPagedPoolUsage = pmc.QuotaPeakNonPagedPoolUsage / DIV / DIV;
			QuotaNonPagedPoolUsage = pmc.PeakWorkingSetSize / DIV / DIV;
			PagefileUsage = pmc.PagefileUsage / DIV / DIV;
			PeakPagefileUsage = pmc.PeakPagefileUsage / DIV / DIV;
		}
		__int64 FileTimeToInt64(const FILETIME& time)
		{
			ULARGE_INTEGER tt;
			tt.LowPart = time.dwLowDateTime;
			tt.HighPart = time.dwHighDateTime;
			return(tt.QuadPart);
		}
	};
	struct GpuInfo
	{
		std::wstring description;
		int DedicatedVideoMemory_M;
		int SharedSystemMemory_M;
		int DedicatedSystemMemory_M;
		UINT VendorId;
		UINT DeviceId;
		UINT SubSysId;
		UINT Revision;
		LUID AdapterLuid;

		std::vector<std::pair<int, int>> avil_ratio;
		int output_device_count;
	};
	struct GpuInfo_Nvml
	{
		char name[NVML_DEVICE_NAME_BUFFER_SIZE];
		unsigned int util_gpu;
		unsigned int util_memory;

		unsigned int total;
		unsigned int free;
		unsigned int used;
		std::map<int, double> proc_mem;
	};
	struct CpuInfo
	{
		wchar_t cpuname[50];
		DWORD mhz;
		std::string chProcessorType;

		int dwNumberOfProcessors;

		char cpu_manufacture[32];
		char cpu_type[32];
		char cpu_freq[32];
	};
	struct OsInfo
	{
		int getsysteminfowarning;
		DWORD majorVersion;
		DWORD minorVersion;
		DWORD buildNumber;
		DWORD platformId;
		WORD serciveMajor;
		WORD serciveMinor;
	};
	struct MemInfo
	{
		unsigned long memoryUsePercent;
		std::pair<int, int> physicalMemory;
		std::pair<int, int> pagingMemory;
		std::pair<int, int> virtualMemory;
		long long extendedMemory;
	};
	struct EventLogObj
	{
		DWORD recNumber;
		unsigned long statusCode;
		DWORD eventId;
		std::wstring normalInfo;
		std::wstring recData;
		std::wstring eventType;
		WCHAR TimeStamp[MAX_TIMESTAMP_LEN];
	};
	struct SystemInfos
	{
		OsInfo osinfo;

		MemInfo meminfo;

		int getcpuinfowarning;
		CpuInfo cpuinfo;

		int getprocessinfowarning;
		
		int monitor_x;
		int monitor_y;
		double monitor_zoom;

		char host_name[255];
		wchar_t username[INFO_BUFFER_SIZE];
		std::vector<char*> networkIps;

		int getgpuinfowarning;
		int GPU_Count;
		std::vector<GpuInfo> GPU_infos;

		int getgpuinfowarningfromnv;
		int getgpuinfonvlasterror;
		std::map<int, std::pair<int, std::vector<std::string>>> getgpuinfolooperrors;

		int getsystemeventlogwarning;
		DWORD getsystemeventloglasterror;
	};
}
#endif