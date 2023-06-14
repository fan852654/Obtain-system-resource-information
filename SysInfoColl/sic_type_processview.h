#pragma once
#ifndef SIC_TYPE_PROCESSVIEW_H
#define SIC_TYPE_PROCESSVIEW_H

#include "sic_defines.h"
#include <tchar.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <Psapi.h>
#pragma comment(lib,"psapi.lib")
namespace SIC_TYPE_PROCESSVIEW {
	enum ProcessViewStatus {
		PVS_NO_ERROR = 0,
		FAILED_TO_CREATE_SNAPSHOT = 0x00000001,
		FAILED_TO_START_FIRSTFUNC = 0x00000002,
		FAILED_TO_OPENTHREADTOKEN = 0x00000004,
		FAILED_TO_IMPERSONATESELF = 0x00000008
	};
	enum ProcessViewDetilStatus
	{
		PVDS_NO_ERROR,
		FAILED_OPEN_PROCESS = 0x00000001,
		FAILED_QUERY_MEMORY_INFO = 0x00000002,
		FAILED_QUERY_TIMES_INFO = 0x00000004,
		FAILED_QUERY_HANDLE_COUNT = 0x00000008
	};
	struct ProcessObj
	{
		int Status;
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
		//句柄数量
		unsigned long HandleCount{ 0 };
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
	typedef struct _Processview {
		_Processview() {
			Status = ProcessViewStatus::PVS_NO_ERROR;
			ProcessCount = 0;
			ThreadsCount = 0;
			HandleCount = 0;
			m_dataLocker = new std::mutex;
		}
		~_Processview() {
			delete m_dataLocker;
			m_dataLocker = nullptr;
		}
		void CopyFrom(const _Processview& _src) {
			this->ProcessDetils.clear();
			this->ServiceDetils.clear();
			std::lock_guard<std::mutex>lc(*_src.m_dataLocker);
			this->Status = _src.Status;
			this->ProcessDetils.insert(this->ProcessDetils.begin(), _src.ProcessDetils.begin(), _src.ProcessDetils.end());
			this->ServiceDetils.insert(this->ServiceDetils.begin(), _src.ServiceDetils.begin(), _src.ServiceDetils.end());
			this->ProcessCount = _src.ProcessCount;
			this->ThreadsCount = _src.ThreadsCount;
			this->HandleCount = _src.HandleCount;
		}
		int Status;
		int ProcessCount;
		unsigned long long ThreadsCount;
		unsigned long long HandleCount;
		std::vector<ProcessObj> ProcessDetils;
		std::vector<ServicesObj> ServiceDetils;
		std::mutex* m_dataLocker;
	}Processview;
	typedef Processview* PProcessview;
}

#endif // !SIC_TYPE_PROCESSVIEW_H
