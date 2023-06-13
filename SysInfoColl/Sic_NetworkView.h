#pragma once
#ifndef SIC_NETWORKVIEW_H
#define SIC_NETWORKVIEW_H

#include "IStrategy.h"
#include <Ras.h>
#include <RasError.h>
#include <tcpmib.h>
#include <IPHlpApi.h>
#include "SicMpiPool.h"
#include "wininet.h"
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(lib, "rasapi32.lib")

#pragma comment(lib, "Wininet.lib")

static std::mutex* m_mutex_network = new std::mutex;
static PNetworkview m_networkview = nullptr;

class Sic_Network : public IStrategy
{
public:
	static Sic_Network* instance;
	static Sic_Network* getInstance();
	Sic_Network();
	~Sic_Network();

	void Execute(void);
	std::string GetErrors(void);
private:
	void GetGlobalNetworkInfo(void);
	void GetHostName(void);
	int GetSystemRasConnections(void);
	BOOL GetWinetProxy(LPSTR lpszProxy, UINT nProxyLen);
	BOOL RefreshNetowrkTables(void);
};


#endif
