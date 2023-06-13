#pragma once
#ifndef ISTRATEGY_H
#define ISTRATEGY_H

#include "sic_types.h"
static std::mutex* m_mutex = new std::mutex;
static SIC_TYPE::PSystemInfos m_sysInfo = nullptr;
class IStrategy
{
public:
	IStrategy() {
		m_mutex->lock();
		if (!m_sysInfo) {
			m_sysInfo = new SIC_TYPE::SystemInfos;
		}
		m_mutex->unlock();
	};
	~IStrategy() {};

	static SIC_TYPE::Sic_return getSystemInfo(SIC_TYPE::SystemInfos** _si) { *_si = m_sysInfo; return SIC_TYPE::SIC_NO_ERROR; };
	virtual void Execute(void) = 0;
	virtual std::string GetErrors(void) = 0;
private:
};
#endif