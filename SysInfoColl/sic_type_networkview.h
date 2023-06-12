#pragma once
#ifndef SIC_TYPE_NETWORKVIEW_H
#define SIC_TYPE_NETWORKVIEW_H

#include <vector>
#include <string>

namespace SIC_TYPE_NETWORKVIEW
{
	enum NetworkViewStatus
	{
		NVS_NO_ERROR,
		FAILED_START_WSA = 0X00000001,
		FAILED_GET_HOSTNAME = 0x00000002,
		FAILED_BEGIN_RASENUMCONNECTION = 0X00000004,
		FAILED_RASENUMCONNECTIONS = 0X00000008,
		FAILED_HEAPALLOC_MEMORY = 0x00000010,
		FAILED_INTERNET_QUERY_OPTION = 0x00000020,
		FAILED_INTERNET_OPENTYPE_ERROR = 0x00000040,
		FAILED_GETWINETPROXY = 0x00000080,
		FAILED_GET_TCP_TABLE = 0X00000100,
		FAILED_MALLOC_MEMORY = 0x00000200,
	};
	typedef enum {
		MIB_TCP_STATE_CLOSED = 1,
		MIB_TCP_STATE_LISTEN = 2,
		MIB_TCP_STATE_SYN_SENT = 3,
		MIB_TCP_STATE_SYN_RCVD = 4,
		MIB_TCP_STATE_ESTAB = 5,
		MIB_TCP_STATE_FIN_WAIT1 = 6,
		MIB_TCP_STATE_FIN_WAIT2 = 7,
		MIB_TCP_STATE_CLOSE_WAIT = 8,
		MIB_TCP_STATE_CLOSING = 9,
		MIB_TCP_STATE_LAST_ACK = 10,
		MIB_TCP_STATE_TIME_WAIT = 11,
		MIB_TCP_STATE_DELETE_TCB = 12,
		MIB_TCP_STATE_RESERVED = 100
	} MIB_TCP_STATE;
	typedef struct _TcpNetworkStatus {
		std::string localAddr;
		std::string remoteAddr;
		unsigned long localPort;
		unsigned long remotePort;
		unsigned long owningPid;
		int Status;
	}TcpNetworkStatus;
	typedef struct _UdpNetworkStatus {
		std::string localAddr;
		unsigned long localPort;
	}UdpNetworkStatus;
	typedef struct _Networkview {
		_Networkview() {
			Status = NetworkViewStatus::NVS_NO_ERROR;
			memset(host_name, 0, 255);
			proxyStr = "";
			RasConnections = 0;
			tcpNetworks = std::vector<TcpNetworkStatus>();
			udpNetworks = std::vector<UdpNetworkStatus>();
		}
		~_Networkview() {
		}
		int Status;
		char host_name[255];
		std::vector<char*> networkIps;
		std::string proxyStr;
		int RasConnections;
		std::vector<TcpNetworkStatus> tcpNetworks;
		std::vector<UdpNetworkStatus> udpNetworks;
	}Networkview;
	typedef Networkview* PNetworkview;
}

#endif