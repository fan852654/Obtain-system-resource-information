#pragma once
#ifndef SIC_OVERVIEW
#define SIC_OVERVIEW

#include "IStrategy.h"
#include "DllFuncDef.h"
#include "DataComm.h"

static std::mutex* m_mutex_overview = new std::mutex;
static POverview m_overview = nullptr;

class Sic_Overview : public IStrategy
{
public:
	static Sic_Overview* instance;
	static Sic_Overview* getInstance();
	Sic_Overview();
	~Sic_Overview();

	void Execute(void);
	std::string GetErrors(void);
private:
	void GetSystemOverView(void);
	void GetVersionInfo(void);
	void GetCpuInfo(void);
};

#endif