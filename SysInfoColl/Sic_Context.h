#pragma once
#ifndef SIC_CONTEXT_H
#define SIC_CONTEXT_H

#include "sic_types.h"
#include "IStrategy.h"
#include "SicMpiPool.h"
#include "Sic_DeviceView.h"
#include "Sic_GraphicsCardView.h"
#include "Sic_Memory.h"
#include "Sic_NetworkView.h"
#include "Sic_Overview.h"
#include "Sic_ProcessView.h"
#include <thread>
#include <map>
#include <chrono>
#include <future>
#include <ctime>

typedef enum SicContextAssembly
{
	Sic_None = 0,
	Sic_OverView = 0x00000001,
	Sic_DeviceView = 0x00000002,
	Sic_GraphicsCardView = 0x00000004,
	Sic_MemoryView = 0x00000008,
	Sic_NetworkView = 0x00000010,
	Sic_ProcessView = 0x00000020,

	Sic_AllAssembly =	Sic_OverView | \
						Sic_DeviceView | \
						Sic_GraphicsCardView | \
						Sic_MemoryView | \
						Sic_NetworkView | \
						Sic_ProcessView 
};

typedef enum SicThreadLoopStatus {
	Loop_None,
	Loop_Begin,
	Loop_Running,
	Loop_Stop
};

typedef struct AssemblyStatus
{
	AssemblyStatus() {
		AssemblyId = SicContextAssembly::Sic_None;
		Executing = false;
		Enabled = false;
		LastRunningTime = 0;
		Interval = 0;
		NeedRefreshLoop = false;
		AssemblyPtr = nullptr;
	};
	unsigned int AssemblyId;
	bool Enabled;
	bool Executing;
	unsigned long long LastRunningTime;
	unsigned long long Interval;
	bool NeedRefreshLoop;
	std::mutex ExecLocker;
	IStrategy* AssemblyPtr;
};

class Sic_Context
{
public:
	Sic_Context(unsigned long _assemblyEnable = SicContextAssembly::Sic_OverView);
	~Sic_Context();
	SIC_TYPE::PSystemInfos GetSystemInfos();

	SIC_TYPE::Sic_return ExecAssemblyOnce(unsigned long _assemblyEnable = SicContextAssembly::Sic_AllAssembly);

	SIC_TYPE::Sic_return EnableRefreshLoops(unsigned long _loopTick);
	SIC_TYPE::Sic_return SetAssemblyLoopTick(unsigned long _assembly, unsigned long _loopTick);
	SIC_TYPE::Sic_return DisableRefreshLoops();
	SIC_TYPE::Sic_return DisableAssemblyLoop(unsigned long _assembly);
	SIC_TYPE::Sic_return SetAssemblyEnable(unsigned long _assemblyEnable,bool _appendToLoops = false);
	SIC_TYPE::Sic_return SetAssemblyDisable(unsigned long _assemblyDisable);

private:
	void Init(void);
	void InitThreadLoop(void);
	void RefreshNeedAssembly(unsigned long _assemblyEnable);
public:
	SicThreadLoopStatus m_AssemblyThreadStatus;
	std::map<SicContextAssembly, AssemblyStatus*> m_AssemblyLoopmap;
private:
	std::mutex m_AssemblyLocker;
	std::thread* m_AssemblyLoopThread{ 0 };
	std::promise<void>* exitSignal{ 0 };
};
static void Exec(Sic_Context* _context,std::future<void>);
#endif