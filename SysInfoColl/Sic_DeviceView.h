#pragma once
#ifndef SIC_DEVICEVIEW_H
#define SIC_DEVICEVIEW_H

#include "IStrategy.h"
#include <SetupAPI.h>
#pragma comment(lib, "setupapi.lib")
#include <vector>
#include <set>

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

	SIC_TYPE::Sic_return GetUnAndInstalledDeives(std::vector<DeviceObj>& _installed, std::vector<DeviceObj>& _uninstalled);
private:
	void GetDeviceList(void);
protected:
};
Sic_DeviceView* Sic_DeviceView::instance = nullptr;

#endif