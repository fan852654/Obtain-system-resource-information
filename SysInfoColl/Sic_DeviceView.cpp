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

SIC_TYPE::Sic_return Sic_DeviceView::GetUnAndInstalledDeives(std::vector<DeviceObj>& _installed, std::vector<DeviceObj>& _uninstalled)
{
	std::vector<DeviceObj> olddevices = std::vector<DeviceObj>(m_deviceview->DevicesVector);
	GetDeviceList();
	int ompNumThreads = SicMpiPool::getInstance()->GetOmpThreadsNumbers();
	std::set<std::wstring> instanceIds;
#pragma omp parallel num_threads(ompNumThreads) private(instanceIds)
	{
#pragma omp sections
		{
#pragma omp section
			{
				for (int i = 0; i < m_deviceview->DevicesVector.size(); i++)
				{
					instanceIds.insert(m_deviceview->DevicesVector[i].instanceId);
				}
				for (int i = 0; i < olddevices.size(); i++)
				{
					if (instanceIds.find(olddevices[i].instanceId) == instanceIds.end())
					{
						_uninstalled.push_back(olddevices[i]);
					}
				}
			}
#pragma omp section
			{
				for (int i = 0; i < olddevices.size(); i++)
				{
					instanceIds.insert(olddevices[i].instanceId);
				}
				for (int i = 0; i < m_deviceview->DevicesVector.size(); i++)
				{
					if (instanceIds.find(m_deviceview->DevicesVector[i].instanceId) == instanceIds.end())
					{
						_installed.push_back(m_deviceview->DevicesVector[i]);
					}
				}
			}
		}
	}
	return SIC_TYPE::Sic_return::SIC_NO_ERROR;
}

void Sic_DeviceView::GetDeviceList(void)
{
	std::vector<DeviceObj> result;
	LPGUID lpGuid = (LPGUID)&GUID_CLASS_USB_HUB;//
	int INTERFACE_DETAIL_SIZE = 1024;
	int nCount = 0;
	HDEVINFO info = SetupDiGetClassDevs(lpGuid, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);

	//enumerate device information  
	DWORD required_size = 0;
	int i = 0;

	DWORD DataT;
	DWORD buffersize = 1024;
	DWORD req_bufsize = 0;

	int ompNumThreads = SicMpiPool::getInstance()->GetOmpThreadsNumbers();
	std::map<int, SP_DEVINFO_DATA> tmp;
	for (int j = 0; j < ompNumThreads; j++)
	{
		tmp[j] = SP_DEVINFO_DATA { sizeof(SP_DEVINFO_DATA) };
	}
#pragma omp parallel num_threads(ompNumThreads) shared(i)
	{
		for (i = 0; SetupDiEnumDeviceInfo(info, i, &(tmp[omp_get_thread_num()])); i++)
		{
			SP_DEVINFO_DATA spdata = tmp[omp_get_thread_num()];
			DeviceObj obj;
			if (!SetupDiGetDeviceInstanceId(info, &spdata, (PTSTR)obj.instanceId, buffersize, &req_bufsize))
				continue;
			if (!SetupDiGetDeviceRegistryPropertyW(info, &spdata, SPDRP_DEVICEDESC, &DataT, (LPBYTE)obj.deviceDesc, buffersize, &req_bufsize))
				continue;
			if (!SetupDiGetDeviceRegistryPropertyW(info, &spdata, SPDRP_FRIENDLYNAME, &DataT, (LPBYTE)obj.friendName, buffersize, &req_bufsize))
			{
				//continue;
			}
			if (!SetupDiGetDeviceRegistryPropertyW(info, &spdata, SPDRP_MFG, &DataT, (LPBYTE)obj.producer, buffersize, &req_bufsize))
			{
				//continue;
			}
			if (!SetupDiGetDeviceRegistryPropertyW(info, &spdata, SPDRP_LOCATION_PATHS, &DataT, (LPBYTE)obj.locationPaths, buffersize, &req_bufsize))
			{
				//continue;
			}
			if (!SetupDiGetDeviceRegistryPropertyW(info, &spdata, SPDRP_LOCATION_INFORMATION, &DataT, (LPBYTE)obj.locationInfo, buffersize, &req_bufsize))
			{
				//continue;
			}
#pragma omp critical
			result.push_back(obj);
		}
	}
	SetupDiDestroyDeviceInfoList(info);
	m_deviceview->DevicesVector.clear();
	m_deviceview->DevicesVector.insert(m_deviceview->DevicesVector.begin(), result.begin(), result.end());
	return;
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
