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
