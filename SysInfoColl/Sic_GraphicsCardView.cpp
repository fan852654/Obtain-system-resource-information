#include "Sic_GraphicsCardView.h"

Sic_GraphicsCardView::Sic_GraphicsCardView()
{
	if (!m_graphicsview) {
		m_graphicsview = new GraphicsCardview;
	}
	m_sysInfo->m_GpuView = m_graphicsview;
}

Sic_GraphicsCardView::~Sic_GraphicsCardView()
{
	if (m_graphicsview) {
		delete m_graphicsview;
		m_graphicsview = nullptr;
	}
	m_sysInfo->m_GpuView = nullptr;
}

void Sic_GraphicsCardView::Execute(void)
{
	GetGpuInfoByNvml();
}

std::string Sic_GraphicsCardView::GetErrors(void)
{
	return std::string();
}

void Sic_GraphicsCardView::GetGpuInfoByNvml(void)
{
	std::vector<GpuInfo_Nvml> result;
	Nvml_Sic::nvmlReturn_enum res;
	unsigned int device_count;
	HINSTANCE nvml_dll = NULL;
	nvml_dll = LoadLibrary(L"nvml.dll");
	if (!nvml_dll) {
		m_graphicsview->Status |= GraphicsCardViewStatus::FAILED_LOAD_NVML_DLL;
		return;
	}
	Nvml_Sic::nvmlInit_nvml nvmlInitFunc = (Nvml_Sic::nvmlInit_nvml)GetProcAddress(nvml_dll, "nvmlInit");
	Nvml_Sic::nvmlShutdown_nvml nvmlShutdownFunc = (Nvml_Sic::nvmlShutdown_nvml)GetProcAddress(nvml_dll, "nvmlShutdown");
	Nvml_Sic::nvmlErrorString_nvml nvmlErrorStringFunc = (Nvml_Sic::nvmlErrorString_nvml)GetProcAddress(nvml_dll, "nvmlErrorString");
	Nvml_Sic::nvmlDeviceGetCount_nvml nvmlDeviceGetCountFunc = (Nvml_Sic::nvmlDeviceGetCount_nvml)GetProcAddress(nvml_dll, "nvmlDeviceGetCount");
	Nvml_Sic::nvmlDeviceGetHandleByIndex_nvml nvmlDeviceGetHandleByIndexFunc = (Nvml_Sic::nvmlDeviceGetHandleByIndex_nvml)GetProcAddress(nvml_dll, "nvmlDeviceGetHandleByIndex");
	Nvml_Sic::nvmlDeviceGetName_nvml nvmlDeviceGetNameFunc = (Nvml_Sic::nvmlDeviceGetName_nvml)GetProcAddress(nvml_dll, "nvmlDeviceGetName");
	Nvml_Sic::nvmlDeviceGetUtilizationRates_nvml nvmlDeviceGetUtilizationRatesFunc = (Nvml_Sic::nvmlDeviceGetUtilizationRates_nvml)GetProcAddress(nvml_dll, "nvmlDeviceGetUtilizationRates");
	Nvml_Sic::nvmlDeviceGetMemoryInfo_nvml nvmlDeviceGetMemoryInfoFunc = (Nvml_Sic::nvmlDeviceGetMemoryInfo_nvml)GetProcAddress(nvml_dll, "nvmlDeviceGetMemoryInfo");
	Nvml_Sic::nvmlDeviceGetComputeRunningProcesses_nvml nvmlDeviceGetComputeRunningProcessesFunc = (Nvml_Sic::nvmlDeviceGetComputeRunningProcesses_nvml)GetProcAddress(nvml_dll, "nvmlDeviceGetComputeRunningProcesses");
	if (!nvmlInitFunc) {
		m_graphicsview->Status |= GraphicsCardViewStatus::FAILED_LOAD_NVMLINITFUNC_;
		return;
	}
	if (!nvmlShutdownFunc) {
		m_graphicsview->Status |= GraphicsCardViewStatus::FAILED_LOAD_NVMLSHUTDOWNFUNC_;
		return;
	}
	if (!nvmlDeviceGetNameFunc) {
		m_graphicsview->Status |= GraphicsCardViewStatus::FAILED_LOAD_NVMLDEVICEGETNAMEFUNC_;
	}
	if (!nvmlDeviceGetUtilizationRatesFunc) {
		m_graphicsview->Status |= GraphicsCardViewStatus::FAILED_LOAD_NVMLDEVICEGETUTILIZATIONTATESFUNC_;
	}
	if (!nvmlDeviceGetMemoryInfoFunc) {
		m_graphicsview->Status |= GraphicsCardViewStatus::FAILED_LOAD_NVMLDEVICEGETMEMORYINFOFUNC_;
	}
	if (!nvmlDeviceGetComputeRunningProcessesFunc) {
		m_graphicsview->Status |= GraphicsCardViewStatus::FAILED_LOAD_NVMLDEVICEGETCOMPUTERUNNINGPROCESSESFUNC_;
	}
	res = nvmlInitFunc();
	if (res != Nvml_Sic::NVML_SUCCESS)
	{
		m_graphicsview->Status |= GraphicsCardViewStatus::FAILED_NVML_INIT;
		goto break_get_gpuinfo_by_nvml;
	}
	if (!nvmlDeviceGetCountFunc) {
		m_graphicsview->Status |= GraphicsCardViewStatus::FAILED_LOAD_NVMLGETCOUNTFUNC_;
		goto break_get_gpuinfo_by_nvml;
	}
	res = nvmlDeviceGetCountFunc(&device_count);
	if (res != Nvml_Sic::NVML_SUCCESS)
	{
		m_graphicsview->Status |= GraphicsCardViewStatus::FAILED_GET_DEVICE_GPU_COUNT;
		goto break_get_gpuinfo_by_nvml;
	}
	if (!nvmlDeviceGetHandleByIndexFunc) {
		m_graphicsview->Status |= GraphicsCardViewStatus::FAILED_LOAD_NVMLGETDEVICEHANDLEBUINDEXFUNC_;
		goto break_get_gpuinfo_by_nvml;
	}
	for (int i = 0; i < device_count; i++)
	{
		Nvml_Sic::nvmlDevice_t device;
		res = nvmlDeviceGetHandleByIndexFunc(i, &device);
		m_graphicsview->GetGpuErrorsInLoops.insert(std::make_pair(i, std::pair<int, std::vector<std::string>>(0, std::vector<std::string>())));
		if (res != Nvml_Sic::NVML_SUCCESS)
		{
			m_graphicsview->GetGpuErrorsInLoops[i].first |= GraphicsCardViewStatus::FAILED_GET_DEVICE_HANDLE;
			m_graphicsview->GetGpuErrorsInLoops[i].second.push_back(std::string(nvmlErrorStringFunc(res)));
			continue;
		}
		GpuInfo_Nvml ginInfo;
		if (nvmlDeviceGetNameFunc) {
			res = nvmlDeviceGetNameFunc(device, ginInfo.name, NVML_DEVICE_NAME_BUFFER_SIZE);
			if (res != Nvml_Sic::NVML_SUCCESS)
			{
				m_graphicsview->GetGpuErrorsInLoops[i].first |= GraphicsCardViewStatus::FAILED_GET_DEVICE_NAME;
				m_graphicsview->GetGpuErrorsInLoops[i].second.push_back(std::string(nvmlErrorStringFunc(res)));
			}
		}

		Nvml_Sic::nvmlUtilization_t utilization{ 0 };
		if (nvmlDeviceGetUtilizationRatesFunc) {
			res = nvmlDeviceGetUtilizationRatesFunc(device, &utilization);
			if (res != Nvml_Sic::NVML_SUCCESS)
			{
				m_graphicsview->GetGpuErrorsInLoops[i].first |= GraphicsCardViewStatus::FAILED_GET_DEVICE_UTILIZATION_RATES;
				m_graphicsview->GetGpuErrorsInLoops[i].second.push_back(std::string(nvmlErrorStringFunc(res)));
			}
			else {
				ginInfo.util_gpu = utilization.gpu;
				ginInfo.util_memory = utilization.memory;
			}
		}
		Nvml_Sic::nvmlMemory_t memory{ 0 };
		if (nvmlDeviceGetMemoryInfoFunc) {
			res = nvmlDeviceGetMemoryInfoFunc(device, &memory);
			if (res != Nvml_Sic::NVML_SUCCESS)
			{
				m_graphicsview->GetGpuErrorsInLoops[i].first |= GraphicsCardViewStatus::FAILED_GET_DEVICE_MEMORY_INFO;
				m_graphicsview->GetGpuErrorsInLoops[i].second.push_back(std::string(nvmlErrorStringFunc(res)));
			}
			else {
				ginInfo.used = memory.used;
				ginInfo.free = memory.free;
				ginInfo.total = memory.total;
			}
		}
		unsigned int infoCnt = 0;
		if (nvmlDeviceGetComputeRunningProcessesFunc) {
			res = nvmlDeviceGetComputeRunningProcessesFunc(device, &infoCnt, nullptr);
			Nvml_Sic::nvmlProcessInfo_t* nvProcInfos = new Nvml_Sic::nvmlProcessInfo_t[infoCnt];
			res = nvmlDeviceGetComputeRunningProcessesFunc(device, &infoCnt, nvProcInfos);
			if (res != Nvml_Sic::NVML_SUCCESS)
			{
				m_graphicsview->GetGpuErrorsInLoops[i].first |= GraphicsCardViewStatus::FAILED_GET_DEVICE_QUERY_RUNNINGPROCESS;
				m_graphicsview->GetGpuErrorsInLoops[i].second.push_back(std::string(nvmlErrorStringFunc(res)));
			}
			else {
				for (int j = 0; j < infoCnt; j++)
				{
					Nvml_Sic::nvmlProcessInfo_t info = nvProcInfos[j];
					ginInfo.proc_mem.insert(std::make_pair(info.pid, info.usedGpuMemory / DIV / DIV));
				}
			}
		}
		result.push_back(ginInfo);
	}
	m_graphicsview->GpuDetils.clear();
	m_graphicsview->GpuDetils.insert(m_graphicsview->GpuDetils.begin(), result.begin(), result.end());
break_get_gpuinfo_by_nvml:
	nvmlShutdownFunc();
	FreeLibrary(nvml_dll);
	return;
}

Sic_GraphicsCardView* Sic_GraphicsCardView::getInstance()
{
	std::lock_guard<std::mutex> lock(*m_mutex_graphics);
	if (instance == nullptr) {
		instance = new Sic_GraphicsCardView;
	}
	return instance;
}