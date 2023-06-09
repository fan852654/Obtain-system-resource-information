#pragma once
#ifndef SIC_PROCESSVIEW_H
#define SIC_PROCESSVIEW_H

#include "IStrategy.h"
#include <tlhelp32.h>
#include <atlbase.h>
#include <map>

static std::mutex* m_mutex_process = new std::mutex;
static PProcessview m_processview = nullptr;

class Sic_ProcessView: public IStrategy
{
public:
	static Sic_ProcessView* instance;
	static Sic_ProcessView* getInstance();
	Sic_ProcessView();
	~Sic_ProcessView();

	void Execute(void);
	std::string GetErrors(void);

	bool SetDebugFlag;
private:
	void GetProcessDetils(void);
	void GetServiceDetils(void);

	SC_HANDLE schSCManager;
protected:
	BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);
};
#endif