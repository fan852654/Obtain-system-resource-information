#include "Sic_DeviceView.h"

Sic_DeviceView::Sic_DeviceView()
{
	if (!m_deviceview) {
		m_deviceview = new Deviceview;
	}
	m_sysInfo->m_DeviceView = m_deviceview;
}

Sic_DeviceView::~Sic_DeviceView()
{
	if (m_deviceview) {
		delete m_deviceview;
		m_deviceview = nullptr;
	}
	m_sysInfo->m_DeviceView = nullptr;
}

void Sic_DeviceView::Execute(void)
{
	GetDeviceList();
}

std::string Sic_DeviceView::GetErrors(void)
{
	return std::string();
}

void Sic_DeviceView::GetDeviceList(void)
{
}

Sic_DeviceView* Sic_DeviceView::getInstance()
{
	m_mutex_device->lock();
	if (instance == nullptr) {
		instance = new Sic_DeviceView;
	}
	m_mutex_device->unlock();
	return instance;
}
