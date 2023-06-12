#pragma once
#ifndef DLLFUNCDEF_H
#define DLLFUNCDEF_H

#include <Windows.h>
#define KERNEL32_DLL	"kernel32"


typedef BOOL(WINAPI* LPFN_GLPI)(
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION,
	PDWORD);

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
	typedef const char* (*nvmlErrorString_nvml)(nvmlReturn_enum);
	typedef nvmlReturn_enum(*nvmlDeviceGetCount_nvml)(unsigned int* deviceCount);
	typedef nvmlReturn_enum(*nvmlDeviceGetHandleByIndex_nvml)(unsigned int index, nvmlDevice_t* device);
	typedef nvmlReturn_enum(*nvmlDeviceGetName_nvml)(nvmlDevice_t device, char* name, unsigned int length);
	typedef nvmlReturn_enum(*nvmlDeviceGetUtilizationRates_nvml)(nvmlDevice_t device, nvmlUtilization_t* utilization);
	typedef nvmlReturn_enum(*nvmlDeviceGetMemoryInfo_nvml)(nvmlDevice_t device, nvmlMemory_t* utilization);
	typedef nvmlReturn_enum(*nvmlDeviceGetComputeRunningProcesses_nvml)(nvmlDevice_t device, unsigned int* infoCount, nvmlProcessInfo_t* infos);
}

#endif // !DLLFUNCDEF_H
