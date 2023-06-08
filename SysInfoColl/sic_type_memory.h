#pragma once
#ifndef SIC_TYPE_MEMORY_H
#define SIC_TYPE_MEMORY_H

#include "sic_defines.h"

namespace SIC_TYPE_MEMORY {
	enum MemoryStatus {
		MS_NO_ERROR = 0,
		MS_CANT_GETGLOBALMEMORYINFO = 0X00000001
	};

	typedef struct _GlobalMemoryInfo {
		int MemoryLoad;
		unsigned long TotalPhysicalMemory;
		unsigned long FreePhysicalMemory;
		unsigned long TotalPagingFile;
		unsigned long FreePagingFile;
		unsigned long TotalVirtualMemory;
		unsigned long FreeVirtualMemory;
	}GlobalMemoryInfo;
	typedef GlobalMemoryInfo* PGlobalMemoryInfo;
	

	typedef struct _Memoryview {
		_Memoryview() {
			Status = MemoryStatus::MS_NO_ERROR;
			m_globalMemory = new GlobalMemoryInfo();
		}
		~_Memoryview() {
			delete m_globalMemory;
		}
		int Status;
		PGlobalMemoryInfo m_globalMemory;
	}Memoryview;
	typedef Memoryview* PMemoryview;
}

#endif