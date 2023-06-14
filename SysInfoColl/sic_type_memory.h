#pragma once
#ifndef SIC_TYPE_MEMORY_H
#define SIC_TYPE_MEMORY_H

#include "sic_defines.h"
#include <mutex>

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
			m_dataLocker = new std::mutex;
		}
		~_Memoryview() {
			delete m_globalMemory;
			m_globalMemory = nullptr;
			delete m_dataLocker;
			m_dataLocker = nullptr;
		}
		void CopyFrom(const _Memoryview& _src) {
			std::lock_guard<std::mutex>lc(*_src.m_dataLocker);
			memcpy_s(this->m_globalMemory, sizeof(GlobalMemoryInfo), _src.m_globalMemory, sizeof(GlobalMemoryInfo));
		}
		int Status;
		PGlobalMemoryInfo m_globalMemory;
		std::mutex* m_dataLocker;
	}Memoryview;
	typedef Memoryview* PMemoryview;
}

#endif