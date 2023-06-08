#pragma once
#ifndef SIC_TYPE_PROCESSVIEW_H
#define SIC_TYPE_PROCESSVIEW_H

#include "sic_defines.h"
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
		FAILED_QUERY_TIMES_INFO = 0x00000004
	};
	struct ProcessObj
	{
		int Status;
		//����id
		DWORD processId;
		//������id
		DWORD parentProcessId;
		//����·��
		WCHAR exeFile[MAX_PATH];
		//���̷�ֵ��������С
		int PeakWorkingSetSize;
		//���̹�������С
		int WorkingSetSize;
		//����ҳ�滺��ط�ֵ
		int QuotaPeakPagedPoolUsage;
		//����ҳ�滺���
		int QuotaPagedPoolUsage;
		//���̷�ҳ�滺��ط�ֵ
		int QuotaPeakNonPagedPoolUsage;
		//���̷�ҳ�滺���
		int QuotaNonPagedPoolUsage;
		//�������������ύ��ҳ
		int PagefileUsage;
		//�������������ύ��ҳ��ֵ
		int PeakPagefileUsage;
		//���̴���ʱ��
		FILETIME lpCreationTime{ 0 };
		//�����˳�ʱ��
		FILETIME lpExitTime{ 0 };
		//����ռ���ں�ʱ��
		FILETIME lpKernelTime{ 0 };
		//����ռ���û�ʱ��
		FILETIME lpUserTime{ 0 };
		//CPUʹ��ʱ��
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
	typedef struct _Processview {
		_Processview() {
			Status = ProcessViewStatus::PVS_NO_ERROR;
			ProcessCount = 0;
			ThreadsCount = 0;
		}
		~_Processview() {
		}
		int Status;
		int ProcessCount;
		long long ThreadsCount;
		std::vector<ProcessObj> ProcessDetils;
	}Processview;
	typedef Processview* PProcessview;
}

#endif // !SIC_TYPE_PROCESSVIEW_H
