#pragma once
#ifndef SIC_TYPE_DEVICEVIEW_H
#define SIC_TYPE_DEVICEVIEW_H

#include "sic_defines.h"

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
		}
		~_Deviceview() {
		}
		int Status;
		std::vector<DeviceObj> DevicesVector;
	}Deviceview;
	typedef Deviceview* PDeviceview;
}

#endif