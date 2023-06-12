#include "Sic_NetworkView.h"

Sic_Network::Sic_Network()
{
	if (!m_networkview) {
		m_networkview = new Networkview;
	}
	m_sysInfo->m_NetworkView = m_networkview;
}

Sic_Network::~Sic_Network()
{
	if (m_networkview) {
		delete m_networkview;
		m_networkview = nullptr;
	}
	m_sysInfo->m_NetworkView = nullptr;
}

void Sic_Network::Execute(void)
{
	GetGlobalNetworkInfo();
}

std::string Sic_Network::GetErrors(void)
{
	return std::string();
}

void Sic_Network::GetGlobalNetworkInfo(void)
{
	this->GetHostName();
	m_networkview->RasConnections = GetSystemRasConnections();
	char proxyChars[4096];
	ZeroMemory(proxyChars, 4096);
	if (GetWinetProxy(proxyChars, 4096)) {
		m_networkview->proxyStr = std::string(proxyChars);
	}
	else {
		m_networkview->Status |= NetworkViewStatus::FAILED_GETWINETPROXY;
	}
	RefreshNetowrkTables();
}

void Sic_Network::GetHostName(void)
{
	WSAData data;
	if (WSAStartup(MAKEWORD(1, 1), &data) != 0) {
		m_networkview->Status |= NetworkViewStatus::FAILED_START_WSA;
	}
	else {
		char host[255];
		struct hostent* p = gethostbyname(NULL);
		if (gethostname(host, sizeof(host)) != SOCKET_ERROR) {
			strcpy(m_networkview->host_name, host);
		}
		else {
			m_networkview->Status |= NetworkViewStatus::FAILED_GET_HOSTNAME;
		}
		if (p)
		{
			for (int i = 0; p->h_addr_list[i] != 0; i++) {
				struct in_addr in;
				memcpy(&in, p->h_addr_list[i], sizeof(struct in_addr));
				m_networkview->networkIps.push_back(inet_ntoa(in));
			}
		}
	}
	WSACleanup();
}

BOOL Sic_Network::GetWinetProxy(LPSTR lpszProxy, UINT nProxyLen)
{
	unsigned long        nSize = 4096;
	char                 szBuf[4096] = { 0 };
	INTERNET_PROXY_INFO* pProxyInfo = (INTERNET_PROXY_INFO*)szBuf;
	if (!InternetQueryOption(NULL, INTERNET_OPTION_PROXY, pProxyInfo, &nSize))
	{
		m_networkview->Status |= NetworkViewStatus::FAILED_INTERNET_QUERY_OPTION;
		return FALSE;
	}
	if (pProxyInfo->dwAccessType == INTERNET_OPEN_TYPE_DIRECT)
	{
		m_networkview->Status |= NetworkViewStatus::FAILED_INTERNET_OPENTYPE_ERROR;
		return FALSE;
	}

	LPCSTR lpszProxyList = (LPCSTR)(pProxyInfo + 1);
	int nLen = strlen(lpszProxyList);
	if (lpszProxy)
	{

		nProxyLen = min(nLen, nProxyLen - 1);
		strncpy_s(lpszProxy, nProxyLen + 1, lpszProxyList, nLen);
		lpszProxy[nProxyLen] = 0;
	}
	return TRUE;
}

BOOL Sic_Network::RefreshNetowrkTables(void)
{
	void* threadToken = nullptr;
	int ompNumThreads = SicMpiPool::getInstance()->GetOmpThreadsNumbers(threadToken, 2);
#pragma omp parallel num_threads(ompNumThreads)
	{
#pragma omp sections
		{
#pragma omp section
			{
				m_networkview->tcpNetworks.clear();
				PMIB_TCPTABLE2 pTcpTable = nullptr;
				DWORD dwSize = 0; 
				DWORD dwRetVal = 0;

				pTcpTable = (MIB_TCPTABLE2*)malloc(sizeof(MIB_TCPTABLE2));
				if (pTcpTable == NULL) {
					m_networkview->Status |= NetworkViewStatus::FAILED_MALLOC_MEMORY;
					goto endofsectionone;
				}
				dwSize = sizeof(MIB_TCPTABLE2);
				if ((dwRetVal = GetTcpTable2(pTcpTable, &dwSize, TRUE)) ==
					ERROR_INSUFFICIENT_BUFFER) {
					free(pTcpTable);
					pTcpTable = (MIB_TCPTABLE2*)malloc(dwSize);
					if (pTcpTable == NULL) {
						m_networkview->Status |= NetworkViewStatus::FAILED_MALLOC_MEMORY;
						goto endofsectionone;
					}
				}
				if ((dwRetVal = GetTcpTable2(pTcpTable, &dwSize, TRUE)) == NO_ERROR)
				{
					struct in_addr IpAddr;
					for (int i = 0; i < (int)pTcpTable->dwNumEntries; i++) {
						char szLocalAddr[128];
						char szRemoteAddr[128];
						IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwLocalAddr;
						strcpy_s(szLocalAddr, sizeof(szLocalAddr), inet_ntoa(IpAddr));
						IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwRemoteAddr;
						strcpy_s(szRemoteAddr, sizeof(szRemoteAddr), inet_ntoa(IpAddr));
						TcpNetworkStatus tns;
						tns.remoteAddr = std::string(szRemoteAddr);
						tns.localAddr = std::string(szLocalAddr);
						tns.remotePort = pTcpTable->table[i].dwRemotePort;
						tns.localPort = pTcpTable->table[i].dwLocalPort;
						tns.owningPid = pTcpTable->table[i].dwOwningPid;
						tns.Status = pTcpTable->table[i].dwState;
						m_networkview->tcpNetworks.push_back(tns);
					}
				}
			endofsectionone:
				if (pTcpTable) {
					free(pTcpTable);
					pTcpTable = nullptr;
				}
			}
#pragma omp section
			{
				m_networkview->tcpNetworks.clear();
				PMIB_UDPTABLE pUdpTable = nullptr;
				DWORD dwSize = 0;
				DWORD dwRetVal = 0;

				pUdpTable = (MIB_UDPTABLE*)malloc(sizeof(MIB_UDPTABLE));
				if (pUdpTable == NULL) {
					m_networkview->Status |= NetworkViewStatus::FAILED_MALLOC_MEMORY;
					goto endofsectiontwo;
				}
				dwSize = sizeof(MIB_UDPTABLE);
				if ((dwRetVal = GetUdpTable(pUdpTable, &dwSize, TRUE)) ==
					ERROR_INSUFFICIENT_BUFFER) {
					free(pUdpTable);
					pUdpTable = (MIB_UDPTABLE*)malloc(dwSize);
					if (pUdpTable == NULL) {
						m_networkview->Status |= NetworkViewStatus::FAILED_MALLOC_MEMORY;
						goto endofsectiontwo;
					}
				}
				if ((dwRetVal = GetUdpTable(pUdpTable, &dwSize, TRUE)) == NO_ERROR)
				{
					struct in_addr IpAddr;
					for (int i = 0; i < (int)pUdpTable->dwNumEntries; i++) {
						char szLocalAddr[128];
						char szRemoteAddr[128];
						IpAddr.S_un.S_addr = (u_long)pUdpTable->table[i].dwLocalAddr;
						strcpy_s(szLocalAddr, sizeof(szLocalAddr), inet_ntoa(IpAddr));
						UdpNetworkStatus uns;
						uns.localAddr = std::string(szLocalAddr);
						uns.localPort = pUdpTable->table[i].dwLocalPort;
						m_networkview->udpNetworks.push_back(uns);
					}
				}
			endofsectiontwo:
				if (pUdpTable) {
					free(pUdpTable);
					pUdpTable = nullptr;
				}
			}
		}
	}
	SicMpiPool::getInstance()->ReleaseOmpThread(threadToken);
	return TRUE;
}

int Sic_Network::GetSystemRasConnections(void)
{
	DWORD dwCb = 0;
	DWORD dwRet = ERROR_SUCCESS;
	DWORD dwConnections = 0;
	LPRASCONN lpRasConn = NULL;

	dwRet = RasEnumConnections(lpRasConn, &dwCb, &dwConnections);
	if (dwRet == ERROR_BUFFER_TOO_SMALL) {
		lpRasConn = (LPRASCONN)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwCb);
		if (lpRasConn == NULL) {
			return dwConnections;
		}else{
			m_networkview->Status |= NetworkViewStatus::FAILED_HEAPALLOC_MEMORY;
		}
		lpRasConn[0].dwSize = sizeof(RASCONN);

		dwRet = RasEnumConnections(lpRasConn, &dwCb, &dwConnections);

		if (ERROR_SUCCESS == dwRet) {
			for (DWORD i = 0; i < dwConnections; i++) {
				wprintf(L"%s\n", lpRasConn[i].szEntryName);
			}
		}
		else {
			m_networkview->Status |= NetworkViewStatus::FAILED_RASENUMCONNECTIONS;
		}
		HeapFree(GetProcessHeap(), 0, lpRasConn);
		lpRasConn = NULL;
		return dwConnections;
	}
	else {
		m_networkview->Status |= NetworkViewStatus::FAILED_BEGIN_RASENUMCONNECTION;
		return 0;
	}
}

Sic_Network* Sic_Network::getInstance()
{
	std::lock_guard<std::mutex> lock(*m_mutex_network);
	if (instance == nullptr) {
		instance = new Sic_Network;
	}
	return instance;
}