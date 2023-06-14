#pragma once
#include "sic_type_overview.h"
#include "sic_type_memory.h"
#include "sic_type_processview.h"
#include "sic_type_deviceview.h"
#include "sic_type_graphicscardview.h"
#include "sic_type_networkview.h"
#ifndef SIC_TYPES_H
#define SIC_TYPES_H

#include "SicMpiPool.h"
#include <mutex>
#include <string>
#include <Windows.h>

using namespace SIC_TYPE_OVERVIEW;
using namespace SIC_TYPE_MEMORY;
using namespace SIC_TYPE_PROCESSVIEW;
using namespace SIC_TYPE_DEVICEVIEW;
using namespace SIC_TYPE_GRAPHICSCARDVIEW;
using namespace SIC_TYPE_NETWORKVIEW;

namespace SIC_TYPE 
{
	enum Sic_return {
		SIC_NO_ERROR = 0
	};

	typedef struct _SystemInfos
	{
		void CopyFrom(const _SystemInfos& _src) {
			m_OverView = new Overview;
			m_OverView->CopyFrom(*_src.m_OverView);
			m_MemoryView = new Memoryview;
			m_MemoryView->CopyFrom(*_src.m_MemoryView);
			m_ProcessView = new Processview;
			m_ProcessView->CopyFrom(*_src.m_ProcessView);
			m_DeviceView = new Deviceview;
			m_DeviceView->CopyFrom(*_src.m_DeviceView);
			m_GpuView = new GraphicsCardview;
			m_GpuView->CopyFrom(*_src.m_GpuView);
			m_NetworkView = new Networkview;
			m_NetworkView->CopyFrom(*_src.m_NetworkView);
		}
		POverview m_OverView{ 0 };
		PMemoryview m_MemoryView{ 0 };
		PProcessview m_ProcessView{ 0 };
		PDeviceview m_DeviceView{ 0 };
		PGraphicsCardview m_GpuView{ 0 };
		PNetworkview m_NetworkView{ 0 };
	}SystemInfos;
	typedef SystemInfos *PSystemInfos;
}

#endif
