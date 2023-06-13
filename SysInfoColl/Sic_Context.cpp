#include "Sic_Context.h"

Sic_Context::Sic_Context(unsigned long _assemblyEnable)
{
	Init();
	RefreshNeedAssembly(_assemblyEnable);
}

Sic_Context::~Sic_Context()
{
	SetAssemblyDisable(SicContextAssembly::Sic_AllAssembly);
}

SIC_TYPE::PSystemInfos Sic_Context::GetSystemInfos()
{
	SIC_TYPE::PSystemInfos* sic = new SIC_TYPE::PSystemInfos();
	if (IStrategy::getSystemInfo(sic) == SIC_TYPE::SIC_NO_ERROR) {
		return (*sic);
	}
	return 0;
}

SIC_TYPE::Sic_return Sic_Context::ExecAssemblyOnce(unsigned long _assemblyEnable)
{
	std::lock_guard<std::mutex> lock(m_AssemblyLocker);
	for (auto it = m_AssemblyLoopmap.begin(); it != m_AssemblyLoopmap.end(); it++)
	{
		if (it->second && it->second->Enabled && (_assemblyEnable & it->first) && it->second->AssemblyPtr) {
			if (!it->second->Executing) {
				it->second->Executing = true;
				std::lock_guard<std::mutex> locker(it->second->ExecLocker);
				it->second->AssemblyPtr->Execute();
				it->second->Executing = false;
			}
		}
	}
	return SIC_TYPE::Sic_return::SIC_NO_ERROR;
}

SIC_TYPE::Sic_return Sic_Context::EnableRefreshLoops(unsigned long _loopTick)
{
	std::lock_guard<std::mutex> lock(m_AssemblyLocker);
	for (auto it = m_AssemblyLoopmap.begin(); it != m_AssemblyLoopmap.end(); it++)
	{
		if (it->second && it->second->Enabled && it->second->AssemblyPtr) {
			it->second->NeedRefreshLoop = true;
			it->second->Interval = _loopTick;
		}
	}
	return SIC_TYPE::Sic_return::SIC_NO_ERROR;
}

SIC_TYPE::Sic_return Sic_Context::SetAssemblyLoopTick(unsigned long _assembly, unsigned long _loopTick)
{
	std::lock_guard<std::mutex> lock(m_AssemblyLocker);
	for (auto it = m_AssemblyLoopmap.begin(); it != m_AssemblyLoopmap.end(); it++)
	{
		if ((_assembly & it->first) && it->second && it->second->Enabled && it->second->AssemblyPtr) {
			it->second->NeedRefreshLoop = true;
			it->second->Interval = _loopTick;
		}
	}
	return SIC_TYPE::Sic_return::SIC_NO_ERROR;
}

SIC_TYPE::Sic_return Sic_Context::DisableRefreshLoops()
{
	std::lock_guard<std::mutex> lock(m_AssemblyLocker);
	for (auto it = m_AssemblyLoopmap.begin(); it != m_AssemblyLoopmap.end(); it++)
	{
		if (it->second && it->second->Enabled && it->second->AssemblyPtr) {
			it->second->NeedRefreshLoop = false;
			it->second->Interval = 0;
		}
	}
	return SIC_TYPE::Sic_return::SIC_NO_ERROR;
}

SIC_TYPE::Sic_return Sic_Context::DisableAssemblyLoop(unsigned long _assembly)
{
	std::lock_guard<std::mutex> lock(m_AssemblyLocker);
	for (auto it = m_AssemblyLoopmap.begin(); it != m_AssemblyLoopmap.end(); it++)
	{
		if ((_assembly & it->first) && it->second && it->second->Enabled && it->second->AssemblyPtr) {
			it->second->NeedRefreshLoop = false;
			it->second->Interval = 0;
		}
	}
	return SIC_TYPE::Sic_return::SIC_NO_ERROR;
}

SIC_TYPE::Sic_return Sic_Context::SetAssemblyEnable(unsigned long _assemblyEnable, bool _appendToLoops)
{
	RefreshNeedAssembly(_assemblyEnable);
	std::lock_guard<std::mutex> lock(m_AssemblyLocker);
	for (auto it = m_AssemblyLoopmap.begin(); it != m_AssemblyLoopmap.end(); it++)
	{
		if ((_assemblyEnable&it->first) && it->second && it->second->Enabled && it->second->AssemblyPtr) {
			it->second->NeedRefreshLoop = _appendToLoops;
		}
	}
	return SIC_TYPE::Sic_return::SIC_NO_ERROR;
}

SIC_TYPE::Sic_return Sic_Context::SetAssemblyDisable(unsigned long _assemblyDisable)
{
	std::lock_guard<std::mutex> lock(m_AssemblyLocker);
	for (auto it = m_AssemblyLoopmap.begin(); it != m_AssemblyLoopmap.end(); it++)
	{
		if ((_assemblyDisable & it->first) && it->second && it->second->Enabled && it->second->AssemblyPtr) {
			it->second->NeedRefreshLoop = false;
			it->second->Enabled = false;
			it->second->Interval = 0;
			it->second->LastRunningTime = 0;
			std::lock_guard<std::mutex>* execlock = new std::lock_guard<std::mutex>(it->second->ExecLocker);
			delete it->second->AssemblyPtr;
			it->second->AssemblyPtr = 0;
			delete execlock;
			delete m_AssemblyLoopmap[it->first];
			m_AssemblyLoopmap[it->first] = nullptr;
		}
	}
	return SIC_TYPE::Sic_return::SIC_NO_ERROR;
}

void Sic_Context::Exec(void)
{
	
}

void Sic_Context::Init(void)
{
	m_AssemblyLoopmap[SicContextAssembly::Sic_OverView] = new AssemblyStatus;
	m_AssemblyLoopmap[SicContextAssembly::Sic_DeviceView] = new AssemblyStatus;
	m_AssemblyLoopmap[SicContextAssembly::Sic_GraphicsCardView] = new AssemblyStatus;
	m_AssemblyLoopmap[SicContextAssembly::Sic_MemoryView] = new AssemblyStatus;
	m_AssemblyLoopmap[SicContextAssembly::Sic_NetworkView] = new AssemblyStatus;
	m_AssemblyLoopmap[SicContextAssembly::Sic_ProcessView] = new AssemblyStatus;
}

void Sic_Context::RefreshNeedAssembly(unsigned long _assemblyEnable)
{
	if ((m_AssemblyLoopmap[SicContextAssembly::Sic_OverView]->Enabled = _assemblyEnable & SicContextAssembly::Sic_OverView) 
		&& !m_AssemblyLoopmap[SicContextAssembly::Sic_OverView]->AssemblyPtr) 
	{
		m_AssemblyLoopmap[SicContextAssembly::Sic_OverView]->AssemblyPtr = Sic_Overview::getInstance();
	}
	if ((m_AssemblyLoopmap[SicContextAssembly::Sic_DeviceView]->Enabled = _assemblyEnable & SicContextAssembly::Sic_DeviceView)
		&& !m_AssemblyLoopmap[SicContextAssembly::Sic_DeviceView]->AssemblyPtr)
	{
		m_AssemblyLoopmap[SicContextAssembly::Sic_DeviceView]->AssemblyPtr = Sic_DeviceView::getInstance();
	}
	if ((m_AssemblyLoopmap[SicContextAssembly::Sic_GraphicsCardView]->Enabled = _assemblyEnable & SicContextAssembly::Sic_GraphicsCardView)
		&& !m_AssemblyLoopmap[SicContextAssembly::Sic_GraphicsCardView]->AssemblyPtr)
	{
		m_AssemblyLoopmap[SicContextAssembly::Sic_GraphicsCardView]->AssemblyPtr = Sic_GraphicsCardView::getInstance();
	}
	if ((m_AssemblyLoopmap[SicContextAssembly::Sic_MemoryView]->Enabled = _assemblyEnable & SicContextAssembly::Sic_MemoryView)
		&& !m_AssemblyLoopmap[SicContextAssembly::Sic_MemoryView]->AssemblyPtr)
	{
		m_AssemblyLoopmap[SicContextAssembly::Sic_MemoryView]->AssemblyPtr = Sic_Memory::getInstance();
	}
	if ((m_AssemblyLoopmap[SicContextAssembly::Sic_NetworkView]->Enabled = _assemblyEnable & SicContextAssembly::Sic_NetworkView)
		&& !m_AssemblyLoopmap[SicContextAssembly::Sic_NetworkView]->AssemblyPtr)
	{
		m_AssemblyLoopmap[SicContextAssembly::Sic_NetworkView]->AssemblyPtr = Sic_Network::getInstance();
	}
	if ((m_AssemblyLoopmap[SicContextAssembly::Sic_ProcessView]->Enabled = _assemblyEnable & SicContextAssembly::Sic_ProcessView)
		&& !m_AssemblyLoopmap[SicContextAssembly::Sic_ProcessView]->AssemblyPtr)
	{
		m_AssemblyLoopmap[SicContextAssembly::Sic_ProcessView]->AssemblyPtr = Sic_ProcessView::getInstance();
	}
}
