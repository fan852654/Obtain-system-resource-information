#pragma once
#ifndef SIC_DEVICEVIEW_H
#define SIC_DEVICEVIEW_H

#include "IStrategy.h"
#include "SicMpiPool.h"

static std::mutex* m_mutex_device = new std::mutex;
static PDeviceview m_deviceview = nullptr;

class Sic_DeviceView :IStrategy
{
public:
	static Sic_DeviceView* instance;
	static Sic_DeviceView* getInstance();
	Sic_DeviceView();
	~Sic_DeviceView();

	void Execute(void);
	std::string GetErrors(void);

private:
	void GetDeviceList(void);
protected:
};
Sic_DeviceView* Sic_DeviceView::instance = nullptr;

#endif