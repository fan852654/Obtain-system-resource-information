#include "Sic_Memory.h"

Sic_Memory::Sic_Memory()
{
	if (!m_memoryview) {
		m_memoryview = new Memoryview;
	}
	m_sysInfo->m_MemoryView = m_memoryview;
}

Sic_Memory::~Sic_Memory()
{
	if (m_memoryview) {
		delete m_memoryview;
		m_memoryview = nullptr;
	}
	m_sysInfo->m_MemoryView = nullptr;
}

void Sic_Memory::Execute(void)
{
	GetGlobalMemoryInfo();
}

std::string Sic_Memory::GetErrors(void)
{
	return std::string();
}

void Sic_Memory::GetGlobalMemoryInfo(void)
{
	MEMORYSTATUSEX statex{};
	statex.dwLength = sizeof(statex);
	if (GlobalMemoryStatusEx(&statex)) {
		m_memoryview->m_globalMemory->MemoryLoad = statex.dwMemoryLoad;
		m_memoryview->m_globalMemory->TotalPhysicalMemory = TWICE_DIVISION_DIV(statex.ullTotalPhys);
		m_memoryview->m_globalMemory->FreePhysicalMemory = TWICE_DIVISION_DIV(statex.ullAvailPhys);
		m_memoryview->m_globalMemory->TotalPagingFile = TWICE_DIVISION_DIV(statex.ullTotalPageFile);
		m_memoryview->m_globalMemory->FreePagingFile = TWICE_DIVISION_DIV(statex.ullAvailPageFile);
		m_memoryview->m_globalMemory->TotalVirtualMemory = TWICE_DIVISION_DIV(statex.ullTotalVirtual);
		m_memoryview->m_globalMemory->FreeVirtualMemory = TWICE_DIVISION_DIV(statex.ullAvailVirtual);
	}
	else {
		m_memoryview->Status |= MemoryStatus::MS_CANT_GETGLOBALMEMORYINFO;
	}
}
Sic_Memory* Sic_Memory::getInstance()
{
	std::lock_guard<std::mutex> lock(*m_mutex_memory);
	if (instance == nullptr) {
		instance = new Sic_Memory;
	}
	return instance;
}