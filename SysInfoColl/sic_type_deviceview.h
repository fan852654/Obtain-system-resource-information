#pragma once
#ifndef SIC_TYPE_DEVICEVIEW_H
#define SIC_TYPE_DEVICEVIEW_H

#include "sic_defines.h"
#include <usbiodef.h>
#include <Usbioctl.h>
#include <map>

namespace SIC_TYPE_DEVICEVIEW
{
	enum DeviceViewStatus
	{
		DVS_NO_ERROR,
	};
	struct DeviceObj
	{
		//�豸����
		wchar_t deviceDesc[1024] = { 0 };
		//�豸ʵ��ID
		wchar_t instanceId[1024] = { 0 };
		//�豸λ��·��
		wchar_t locationPaths[1024] = { 0 };
		//�豸λ����Ϣ
		wchar_t locationInfo[1024] = { 0 };
		//�豸�Ѻ�����
		wchar_t friendName[1024] = { 0 };
		//�豸������
		wchar_t producer[1024] = { 0 };
	};
	typedef struct _Deviceview {
		_Deviceview() {
			Status = DeviceViewStatus::DVS_NO_ERROR;
			DevicesVector = std::vector<DeviceObj>();
			m_dataLocker = new std::mutex;
		}
		~_Deviceview() {
			delete m_dataLocker;
			m_dataLocker = nullptr;
		}
		void CopyFrom(const _Deviceview& _src) {
			this->DevicesVector.clear();
			std::lock_guard<std::mutex>lc(*_src.m_dataLocker);
			this->Status = _src.Status;
			this->DevicesVector.insert(this->DevicesVector.begin(), _src.DevicesVector.begin(), _src.DevicesVector.end());
		}
		int Status;
		std::vector<DeviceObj> DevicesVector;
		std::mutex* m_dataLocker;
	}Deviceview;
	typedef Deviceview* PDeviceview;
}

#endif