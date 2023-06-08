#pragma once
#ifndef SIC_TYPE_OVERVIEW_H
#define SIC_TYPE_OVERVIEW_H

#include <Windows.h>
#include "sic_defines.h"
namespace SIC_TYPE_OVERVIEW {

	enum OverViewStatus {
		OVS_NO_ERROR = 0,
		OVS_CANNOT_GET_VERSION = 0X00000001,
		OVS_CANNOT_GET_FUNC_GETLOGICALPROCESSORINFORM = 0X00000002,
		OVS_CANNOT_ALLOCATION_MEM = 0X00000004,
		OVS_CANNOT_GET_PHYSICALLYMEMORYINSTALLED = 0X00000008
	};
	typedef struct _SysRunningInfo {
		ULONGLONG Up_time;
		wchar_t username[INFO_BUFFER_SIZE];
		int physicallyMemoryInstalled;
	}SysRunningInfo;
	typedef struct _OsInfo {
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
	}OsInfo;
	typedef struct _CpuInfo {
		int CPUCoreCount;
		int LastError;
	#ifdef WIN32
		int numaNodeCount;
		int processorPackageCount;
		int processorCoreCount;
		int logicalProcessorCount;
		int processorL1CacheCount;
		int processorL2CacheCount;
		int processorL3CacheCount;
	#else
		int enableCoreCount;
	#endif
	}CpuInfo;
	typedef struct _Overview {
		_Overview() {
			Status = OverViewStatus::OVS_NO_ERROR;
			m_osInfo = new OsInfo;
			m_cpuInfo = new CpuInfo;
			m_runInfo = new SysRunningInfo;
		}
		~_Overview() {
			delete m_osInfo;
			delete m_cpuInfo;
			delete m_runInfo;
		}
		int Status;
		OsInfo* m_osInfo;
		CpuInfo* m_cpuInfo;
		SysRunningInfo* m_runInfo;
	}Overview;
	typedef Overview* POverview;

}
#endif // !SIC_TYPE_OVERVIEW_H
