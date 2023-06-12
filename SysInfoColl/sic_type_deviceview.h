#pragma once
#ifndef SIC_TYPE_DEVICEVIEW_H
#define SIC_TYPE_DEVICEVIEW_H

#include "sic_defines.h"
#include <map>

DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE,
	0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED);
#define   GUID_CLASS_USB_DEVICE                   GUID_DEVINTERFACE_USB_DEVICE  
DEFINE_GUID(GUID_DEVINTERFACE_USB_HUB,
	0xf18a0e88, 0xc30c, 0x11d0, 0x88, 0x15, 0x00, 0xa0, 0xc9, 0x06, 0xbe, 0xd8);

#define		GUID_CLASS_USB_HUB GUID_DEVINTERFACE_USB_HUB
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