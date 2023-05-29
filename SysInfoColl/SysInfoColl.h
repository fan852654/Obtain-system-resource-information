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
	/// ��ȡ�豸�б�
	/// ��һ��ʹ�û�õ�_installedDev�ǵ�ǰϵͳ�����豸�б�
	/// ����Ϊ��һ��ʹ�ú���֮��ĸ���
	/// </summary>
	/// <param name="_installedDev">�ϴ�ʹ�ú���֮����ӵ��豸</param>
	/// <param name="_uninstalledDev">�ϴ�ʹ�ú���֮��ж�ص��豸</param>
	void GetDeviceList(std::vector<DeviceObj>& _installedDev, std::vector<DeviceObj>& _uninstalledDev);
	/// <summary>
	/// ��ȡ�¼���־
	/// </summary>
	/// <param name="timeTick">��ȡ���ڵ�timeTickʱ��֮�ڵ���־</param>
	/// <param name="eventName">��ȡ����־�����Application��System</param>
	/// <returns></returns>
	std::vector<EventLogObj> GetSystemEventLog(unsigned long timeTick,std::wstring eventName = L"Application");
	/// <summary>
	/// ��ȡϵͳ��ϸ��Ϣ
	/// �ᱣ����һ���ṹ���У�ֱ�ӵ���
	/// </summary>
	void GetSystemDetils();
	/// <summary>
	/// ��ȡ�����б�
	/// </summary>
	/// <param name="setdebugflag">�Ƿ�����SE_DEBUG_NAMEģʽ</param>
	/// <returns></returns>
	std::vector<ProcessObj> GetProcessDetils(bool setdebugflag = false);
	/// <summary>
	/// ��ȡϵͳ�д��ڵķ����б�
	/// </summary>
	/// <returns></returns>
	std::vector<ServicesObj> GetSystemServicesList();
	/// <summary>
	/// ͨ��nvml��ȡGPU��Ϣ��ÿ��GPU���������еĽ��̼�����ռ���ڴ�ռ�
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