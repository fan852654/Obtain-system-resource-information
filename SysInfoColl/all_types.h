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

//#include <cuda_runtime_api.h>
//#pragma comment(lib,"cuda.lib")
//#pragma comment(lib,"cudadevrt.lib")
//#pragma comment(lib,"cudart.lib")
//#pragma comment(lib,"cudart_static.lib")
//#pragma comment(lib,"cublas.lib")
//#pragma comment(lib,"cublasLt.lib")
//#include <nvml.h>
//#pragma comment(lib,"nvml.lib")


#include <DXGI.h>
#pragma comment(lib,"DXGI.lib")
#include <strsafe.h>

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
#define NVML_DEVICE_NAME_BUFFER_SIZE                  64
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
	enum EventLogEnum 
	{
		EV_SYSTEM,
		EV_APPLICATION,
		EV_SECURITY
	};
	enum GetCPUInfoWarning
	{
		GCIW_NO_ERROR,
		FAILED_TO_OPEN_REGEDIT = 0x00000001,
		FAILED_TO_QUERY_PROCESSNAMESTRING = 0x00000010,
		FAILED_TO_QUERY_MHZ = 0x00000100
	};
	enum GetGPUInfoWarning
	{
		GGIW_NO_ERROR,
		CREATE_DXGIFACTORY_FAILED = 0x00000001,
		FAILED_NVML_INIT = 0x00000010,
		FAILED_GET_DEVICE_GPU_COUNT = 0x00000100,
		FAILED_GET_DEVICE_HANDLE = 0x00001000,
		FAILED_GET_DEVICE_NAME = 0x00010000,
		FAILED_GET_DEVICE_UTILIZATION_RATES = 0x00100000,
		FAILED_GET_DEVICE_MEMORY_INFO = 0x01000000,
		FAILED_GET_DEVICE_QUERY_RUNNINGPROCESS = 0x10000000,
		FAILED_LOAD_NVML_DLL = 0x0000000a,
		FAILED_LOAD_NVMLINITFUNC_ = 0x000000a0,
		FAILED_LOAD_NVMLSHUTDOWNFUNC_ = 0x000000b0,
		FAILED_LOAD_NVMLGETCOUNTFUNC_ = 0x00000a00,
		FAILED_LOAD_NVMLGETDEVICEHANDLEBUINDEXFUNC_ = 0x0000a000,
		FAILED_LOAD_NVMLDEVICEGETNAMEFUNC_ = 0x000a0000,
		FAILED_LOAD_NVMLDEVICEGETUTILIZATIONTATESFUNC_ = 0x00a00000,
		FAILED_LOAD_NVMLDEVICEGETMEMORYINFOFUNC_ = 0x0a000000,
		FAILED_LOAD_NVMLDEVICEGETCOMPUTERUNNINGPROCESSESFUNC_ = 0xa0000000,
	};
	enum GetProcessInfoWarning
	{
		GPIW_NO_ERROR,
		FAILED_TO_CREATE_SNAPSHOT = 0x00000001,
		FAILED_TO_START_FIRSTFUNC = 0x00000010,
		FAILED_TO_OPENTHREADTOKEN = 0x00000100,
		FAILED_TO_IMPERSONATESELF = 0x00001000
	};
	enum GetProcessDetilsWarning
	{
		GPDW_NO_ERROR,
		FAILED_OPEN_PROCESS = 0x00000001,
		FAILED_QUERY_MEMORY_INFO = 0x00000010,
		FAILED_QUERY_TIMES_INFO = 0x00000100
	};
	enum GetEventLogWarning
	{
		GELW_NO_ERROR,
		FAILED_NO_ENOUGH_MEMORY = 0x00000001,
		FAILED_REALLOC_MEMORY = 0x00000010,
		FAILED_OPEN_EVENT_LOG = 0x00000100
	};
	struct ServicesObj
	{
		//服务进程名
		WCHAR* m_name;
		//服务进程id
		unsigned int m_pid = 0;
		//服务显示名称
		std::wstring m_dispName;
		//服务状态
		unsigned int m_state = 0;
		std::wstring  m_group;
		std::wstring  m_path;
	};
	struct DeviceObj
	{
		//设备描述
		wchar_t deviceDesc[1024] = { 0 };
		//设备实例ID
		wchar_t instanceId[1024] = { 0 };
		//设备位置路径
		wchar_t locationPaths[1024] = { 0 };
		//设备位置信息
		wchar_t locationInfo[1024] = { 0 };
		//设备友好命名
		wchar_t friendName[1024] = { 0 };
		//设备制造商
		wchar_t producer[1024] = { 0 };
	};
	struct ProcessObj
	{
		int getprocessdetilswarning;
		//进程id
		DWORD processId;
		//父进程id
		DWORD parentProcessId;
		//进程路径
		WCHAR exeFile[MAX_PATH];
		//进程峰值工作集大小
		int PeakWorkingSetSize;
		//进程工作集大小
		int WorkingSetSize;
		//进程页面缓存池峰值
		int QuotaPeakPagedPoolUsage;
		//进程页面缓存池
		int QuotaPagedPoolUsage;
		//进程非页面缓冲池峰值
		int QuotaPeakNonPagedPoolUsage;
		//进程非页面缓冲池
		int QuotaNonPagedPoolUsage;
		//进程生存期内提交的页
		int PagefileUsage;
		//进程生存期内提交的页峰值
		int PeakPagefileUsage;
		//进程创建时间
		FILETIME lpCreationTime{ 0 };
		//进程退出时间
		FILETIME lpExitTime{ 0 };
		//进程占用内核时间
		FILETIME lpKernelTime{ 0 };
		//进程占用用户时间
		FILETIME lpUserTime{ 0 };
		//CPU使用时间
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
		//GPU描述
		std::wstring description;
		//专用视频内存
		int DedicatedVideoMemory_M;
		//共享系统内存
		int SharedSystemMemory_M;
		//专用系统内存
		int DedicatedSystemMemory_M;
		//硬件供应商PCI ID
		UINT VendorId;
		//硬件设备PCI ID
		UINT DeviceId;
		//子系统PCI ID
		UINT SubSysId;
		//适配器修订版号PCI ID
		UINT Revision;
		//适配器唯一值
		LUID AdapterLuid;
		//适配器的输出分辨率
		std::vector<std::pair<int, int>> avil_ratio;
		int output_device_count;
	};

	struct GpuInfo_Nvml
	{
		char name[NVML_DEVICE_NAME_BUFFER_SIZE];
		//一个/多个内核在GPU上执行的时间百分比
		unsigned int util_gpu;
		//内存容量
		unsigned int util_memory;
		//总内存
		unsigned int total;
		//空闲内存
		unsigned int free;
		//使用中内存
		unsigned int used;
		//每个应用程序使用的内存
		std::map<int/*进程id*/, double/*显存*/> proc_mem;
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
		//系统主要版本号
		DWORD majorVersion;
		//系统次要版本号
		DWORD minorVersion;
		//内部版本号
		DWORD buildNumber;
		//系统平台
		DWORD platformId;
		//sp主版本号 类似 service pack
		WORD serciveMajor;
		//sp次要版本号
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
		//分辨率x
		int monitor_x;
		//分辨率y
		int monitor_y;
		//缩放比
		double monitor_zoom;

		char host_name[255];
		wchar_t username[INFO_BUFFER_SIZE];
		std::vector<char*> networkIps;

		int getgpuinfowarning;
		int GPU_Count;
		std::vector<GpuInfo> GPU_infos;
		std::string DirectXVersion;

		int getgpuinfowarningfromnv;
		int getgpuinfonvlasterror;
		std::map<int, std::pair<int, std::vector<std::string>>> getgpuinfolooperrors;

		int getsystemeventlogwarning;
		DWORD getsystemeventloglasterror;
	};
	typedef UINT(*GetDpiForWindow_USER32)(HWND hwnd);
}

namespace Nvml_Sic
{
	typedef enum nvmlReturn_enum
	{
		// cppcheck-suppress *
		NVML_SUCCESS = 0,                          //!< The operation was successful
		NVML_ERROR_UNINITIALIZED = 1,              //!< NVML was not first initialized with nvmlInit()
		NVML_ERROR_INVALID_ARGUMENT = 2,           //!< A supplied argument is invalid
		NVML_ERROR_NOT_SUPPORTED = 3,              //!< The requested operation is not available on target device
		NVML_ERROR_NO_PERMISSION = 4,              //!< The current user does not have permission for operation
		NVML_ERROR_ALREADY_INITIALIZED = 5,        //!< Deprecated: Multiple initializations are now allowed through ref counting
		NVML_ERROR_NOT_FOUND = 6,                  //!< A query to find an object was unsuccessful
		NVML_ERROR_INSUFFICIENT_SIZE = 7,          //!< An input argument is not large enough
		NVML_ERROR_INSUFFICIENT_POWER = 8,         //!< A device's external power cables are not properly attached
		NVML_ERROR_DRIVER_NOT_LOADED = 9,          //!< NVIDIA driver is not loaded
		NVML_ERROR_TIMEOUT = 10,                   //!< User provided timeout passed
		NVML_ERROR_IRQ_ISSUE = 11,                 //!< NVIDIA Kernel detected an interrupt issue with a GPU
		NVML_ERROR_LIBRARY_NOT_FOUND = 12,         //!< NVML Shared Library couldn't be found or loaded
		NVML_ERROR_FUNCTION_NOT_FOUND = 13,        //!< Local version of NVML doesn't implement this function
		NVML_ERROR_CORRUPTED_INFOROM = 14,         //!< infoROM is corrupted
		NVML_ERROR_GPU_IS_LOST = 15,               //!< The GPU has fallen off the bus or has otherwise become inaccessible
		NVML_ERROR_RESET_REQUIRED = 16,            //!< The GPU requires a reset before it can be used again
		NVML_ERROR_OPERATING_SYSTEM = 17,          //!< The GPU control device has been blocked by the operating system/cgroups
		NVML_ERROR_LIB_RM_VERSION_MISMATCH = 18,   //!< RM detects a driver/library version mismatch
		NVML_ERROR_IN_USE = 19,                    //!< An operation cannot be performed because the GPU is currently in use
		NVML_ERROR_MEMORY = 20,                    //!< Insufficient memory
		NVML_ERROR_NO_DATA = 21,                   //!< No data
		NVML_ERROR_VGPU_ECC_NOT_SUPPORTED = 22,    //!< The requested vgpu operation is not available on target device, becasue ECC is enabled
		NVML_ERROR_INSUFFICIENT_RESOURCES = 23,    //!< Ran out of critical resources, other than memory
		NVML_ERROR_FREQ_NOT_SUPPORTED = 24,        //!< Ran out of critical resources, other than memory
		NVML_ERROR_ARGUMENT_VERSION_MISMATCH = 25, //!< The provided version is invalid/unsupported
		NVML_ERROR_DEPRECATED = 26,	           //!< The requested functionality has been deprecated
		NVML_ERROR_UNKNOWN = 999                   //!< An internal driver error occurred
	} nvmlReturn_t; 
	typedef struct nvmlDevice_st* nvmlDevice_t;
	typedef struct nvmlUtilization_st
	{
		unsigned int gpu;
		unsigned int memory;
	} nvmlUtilization_t;
	typedef struct nvmlMemory_st
	{
		unsigned long long total;
		unsigned long long free;
		unsigned long long used;
	} nvmlMemory_t; 
	typedef struct nvmlProcessInfo_st
	{
		unsigned int        pid;
		unsigned long long  usedGpuMemory;
		unsigned int        gpuInstanceId;
		unsigned int        computeInstanceId;
	} nvmlProcessInfo_t;
	typedef nvmlReturn_enum(*nvmlInit_nvml)();
	typedef nvmlReturn_enum(*nvmlShutdown_nvml)(void);
	typedef const char*(*nvmlErrorString_nvml)(nvmlReturn_enum);
	typedef nvmlReturn_enum(*nvmlDeviceGetCount_nvml)(unsigned int* deviceCount);
	typedef nvmlReturn_enum(*nvmlDeviceGetHandleByIndex_nvml)(unsigned int index, nvmlDevice_t* device);
	typedef nvmlReturn_enum(*nvmlDeviceGetName_nvml)(nvmlDevice_t device, char* name, unsigned int length);
	typedef nvmlReturn_enum(*nvmlDeviceGetUtilizationRates_nvml)(nvmlDevice_t device, nvmlUtilization_t* utilization);
	typedef nvmlReturn_enum(*nvmlDeviceGetMemoryInfo_nvml)(nvmlDevice_t device, nvmlMemory_t* utilization);
	typedef nvmlReturn_enum(*nvmlDeviceGetComputeRunningProcesses_nvml)(nvmlDevice_t device, unsigned int* infoCount, nvmlProcessInfo_t* infos);
}

#endif