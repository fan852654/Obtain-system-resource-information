#pragma once
#ifndef SIC_MEMORY_H
#define SIC_MEMORY_H

#include "IStrategy.h"
#ifdef WIN32
#include <WbemIdl.h>
# pragma comment(lib, "wbemuuid.lib")
#endif

static std::mutex* m_mutex_memory = new std::mutex;
static PMemoryview m_memoryview = nullptr;

class Sic_Memory :IStrategy
{
public:
	static Sic_Memory* instance;
	static Sic_Memory* getInstance();
	Sic_Memory();
	~Sic_Memory();

	void Execute(void);
	std::string GetErrors(void);
private:
	void GetGlobalMemoryInfo(void);
};
Sic_Memory* Sic_Memory::instance = nullptr;


#endif
