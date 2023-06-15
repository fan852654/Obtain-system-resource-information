// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "MemoryPool.h"
#include "SicMpiPool.h"
#include "Sic_DeviceView.h"
#include "Sic_GraphicsCardView.h"
#include "Sic_Memory.h"
#include "Sic_NetworkView.h"
#include "Sic_Overview.h"
#include "Sic_ProcessView.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

MemoryPool* MemoryPool::instance = nullptr;
SicMpiPool* SicMpiPool::instancePool = nullptr;
Sic_ProcessView* Sic_ProcessView::instance = nullptr;
Sic_Overview* Sic_Overview::instance = nullptr;
Sic_Network* Sic_Network::instance = nullptr;
Sic_Memory* Sic_Memory::instance = nullptr;
Sic_GraphicsCardView* Sic_GraphicsCardView::instance = nullptr;
Sic_DeviceView* Sic_DeviceView::instance = nullptr;