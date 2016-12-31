#pragma once

const bool NETWORK_ENABLED = false;
static char SERVER_ADDRESS[] = "SANKI";
const unsigned short SERVER_PORT = 0;

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")

#include <winsock2.h>
#include <mmsystem.h>

#include "NetworkMessages.h"

class NetworkClass
{
public:

	NetworkClass();
	NetworkClass(const NetworkClass&);
	~NetworkClass();

	bool Initialize();
	void Shutdown();
	void Frame();

	int GetLatency();
	bool ConnectToServer(char*, unsigned short);

	void SetThreadActive();
	void SetThreadInactive();
	bool Online();
	SOCKET GetClientSocket();
	void ReadNetworkMessage(char*);

private:

	bool InitializeWinSock();
	void ShutdownWinSock();
	void ProcessLatency();
	void SendPing();
	void HandlePingMessage();
	void SendDisconnectMessage();

private:

	SOCKET m_clientSocket;
	int m_addressLength;
	struct sockaddr_in m_serverAddress;
	short m_idNumber;
	bool m_online;
	bool m_threadActive;
	int m_latency;
	unsigned long m_pingTime;
};

void NetworkReadFunction(void*);