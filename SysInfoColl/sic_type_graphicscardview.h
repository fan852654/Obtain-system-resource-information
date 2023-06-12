#pragma once
#ifndef SIC_TYPE_GRAPHICSCARDVIEW_H
#define SIC_TYPE_GRAPHICSCARDVIEW_H

#include <string>
#include <vector>
#include <map>
#include "sic_defines.h"
namespace SIC_TYPE_GRAPHICSCARDVIEW {
	enum GraphicsCardViewStatus {
		GCVS_NO_ERROR = 0,
		CREATE_DXGIFACTORY_FAILED = 0x00000001,
		FAILED_NVML_INIT = 0x00000002,
		FAILED_GET_DEVICE_GPU_COUNT = 0x00000004,
		FAILED_GET_DEVICE_HANDLE = 0x00000008,
		FAILED_GET_DEVICE_NAME = 0x00000010,
		FAILED_GET_DEVICE_UTILIZATION_RATES = 0x00000020,
		FAILED_GET_DEVICE_MEMORY_INFO = 0x00000040,
		FAILED_GET_DEVICE_QUERY_RUNNINGPROCESS = 0x00000080,
		FAILED_LOAD_NVML_DLL = 0x00000100,
		FAILED_LOAD_NVMLINITFUNC_ = 0x00000200,
		FAILED_LOAD_NVMLSHUTDOWNFUNC_ = 0x00000400,
		FAILED_LOAD_NVMLGETCOUNTFUNC_ = 0x00000800,
		FAILED_LOAD_NVMLGETDEVICEHANDLEBUINDEXFUNC_ = 0x00010000,
		FAILED_LOAD_NVMLDEVICEGETNAMEFUNC_ = 0x00002000,
		FAILED_LOAD_NVMLDEVICEGETUTILIZATIONTATESFUNC_ = 0x00004000,
		FAILED_LOAD_NVMLDEVICEGETMEMORYINFOFUNC_ = 0x00008000,
		FAILED_LOAD_NVMLDEVICEGETCOMPUTERUNNINGPROCESSESFUNC_ = 0x00010000,
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
	typedef struct _GraphicsCardview {
		_GraphicsCardview() {
			Status = GraphicsCardViewStatus::GCVS_NO_ERROR;
			GpuDetils = std::vector<GpuInfo_Nvml>();
		}
		~_GraphicsCardview() {
		}
		int Status;
		std::map<int, std::pair<int, std::vector<std::string>>> GetGpuErrorsInLoops;
		std::vector<GpuInfo_Nvml> GpuDetils;
	}GraphicsCardview;
	typedef GraphicsCardview* PGraphicsCardview;
}

#endif
