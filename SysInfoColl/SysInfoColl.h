#pragma once


#ifdef DLL_SYSINFOCOLLECTER_API
#define DLL_SYSINFOCOLLECTER_API __declspec(dllexport)
#else
#define DLL_SYSINFOCOLLECTER_API __declspec(dllimport)
#endif

#ifndef SYSINFOCOLLECTER_H
#define SYSINFOCOLLECTER_H

#include "all_types.h"
#include "DataComm.h"
using namespace DT;

class DLL_SYSINFOCOLLECTER_API SysInfoColl
{
public:
	SysInfoColl();
	~SysInfoColl();
	/// <summary>
	/// 获取设备列表
	/// 第一次使用获得的_installedDev是当前系统所有设备列表
	/// 后续为上一次使用函数之后的更新
	/// </summary>
	/// <param name="_installedDev">上次使用函数之后添加的设备</param>
	/// <param name="_uninstalledDev">上次使用函数之后卸载的设备</param>
	void GetDeviceList(std::vector<DeviceObj>& _installedDev, std::vector<DeviceObj>& _uninstalledDev);
	/// <summary>
	/// 获取事件日志
	/// </summary>
	/// <param name="timeTick">获取现在到timeTick时间之内的日志</param>
	/// <param name="eventName">获取的日志类别如Application，System</param>
	/// <returns></returns>
	std::vector<EventLogObj> GetSystemEventLog(unsigned long timeTick,std::wstring eventName = L"Application");
	/// <summary>
	/// 获取系统详细信息
	/// 会保存至一个结构体中，直接调用
	/// </summary>
	void GetSystemDetils();
	/// <summary>
	/// 获取进程列表
	/// </summary>
	/// <param name="setdebugflag">是否设置SE_DEBUG_NAME模式</param>
	/// <returns></returns>
	std::vector<ProcessObj> GetProcessDetils(bool setdebugflag = false);
	/// <summary>
	/// 获取系统中存在的服务列表
	/// </summary>
	/// <returns></returns>
	std::vector<ServicesObj> GetSystemServicesList();
	/// <summary>
	/// 通过nvml获取GPU信息及每个GPU中正在运行的进程及进程占用内存空间
	/// </summary>
	/// <returns></returns>
	std::vector<GpuInfo_Nvml> GetGpuInfoByNvml();
protected:
	std::vector<DeviceObj> GetAllDevices();
	BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);
	void GetTimestamp(const DWORD Time, WCHAR DisplayString[]);
private:
	DT::SystemInfos m_sysinfo;

	SC_HANDLE schSCManager;
	MonitorColType m_mct;

	std::vector<DeviceObj> m_devices;
};

#endif