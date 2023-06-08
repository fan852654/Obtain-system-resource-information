#pragma once
#ifndef SIC_PROCESSVIEW_H
#define SIC_PROCESSVIEW_H

#include "IStrategy.h"

static std::mutex* m_mutex_process = new std::mutex;
static PProcessview m_processview = nullptr;

class Sic_ProcessView:IStrategy
{
public:
	static Sic_ProcessView* instance;
	static Sic_ProcessView* getInstance();
	Sic_ProcessView();
	~Sic_ProcessView();

	void Execute(void);
	std::string GetErrors(void);
private:
	void GetProcessDetils(void);
};
Sic_ProcessView* Sic_ProcessView::instance = nullptr;
#endif