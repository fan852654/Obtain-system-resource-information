#pragma once

#ifndef SYSINFOCOLLECTER_H
#define SYSINFOCOLLECTER_H

#include "all_types.h"
#include "DataComm.h"
using namespace DT;

class SysInfoCollecter
{
public:
	SysInfoCollecter();
	~SysInfoCollecter();

	void GetDeviceList(std::vector<DeviceObj>& _installedDev, std::vector<DeviceObj>& _uninstalledDev);
	void GetSystemEventLog();
public:
	void GetSystemDetils();
	void RegUsbEvent();
	std::vector<ProcessObj> GetProcessDetils(bool setdebugflag = false);
	std::vector<ServicesObj> GetSystemServicesList();
	std::vector<GpuInfo_Nvml> GetGpuInfoByNvml();
	std::vector<DeviceObj> GetAllDevices();
	BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);
private:
	DT::SystemInfos m_sysinfo;

	SC_HANDLE schSCManager;
	MonitorColType m_mct;

	std::vector<DeviceObj> m_devices;
};

#endif