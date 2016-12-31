#include "NetworkClass.h"

NetworkClass::NetworkClass()
{
	m_online = false;
}

NetworkClass::NetworkClass(const NetworkClass& other)
{
}

NetworkClass::~NetworkClass()
{
}

bool NetworkClass::Initialize()
{
	bool result;

	result = InitializeWinSock();

	if(!result)
	{
		return false;
	}

	return true;
}

void NetworkClass::Shutdown()
{
	if(!m_online)
	{
		return;
	}

	SendDisconnectMessage();

	m_online = false;

	while(m_threadActive)
	{
	}

	closesocket(m_clientSocket);

	ShutdownWinSock();

	return;
}

void NetworkClass::Frame()
{
	ProcessLatency();

	return;
}

int NetworkClass::GetLatency()
{
	return m_latency;
}

bool NetworkClass::ConnectToServer(char* ipAddress, unsigned short portNumber)
{
	unsigned long setting, inetAddress, startTime, threadId;
	MSG_GENERIC_DATA connectMessage, *message;
	int error, bytesSent, bytesRead;
	bool done, gotId;
	char recvBuffer[4096];
	HANDLE threadHandle;

	m_clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

	if(m_clientSocket == INVALID_SOCKET)
	{
		return false;
	}

	setting = 1;
	error = ioctlsocket(m_clientSocket, FIONBIO, & setting);

	if(error == SOCKET_ERROR)
	{
		return false;
	}

	m_addressLength = sizeof(m_serverAddress);

	inetAddress = inet_addr(ipAddress);
	memset((char*)&m_serverAddress, 0, m_addressLength);
	m_serverAddress.sin_family = AF_INET;
	m_serverAddress.sin_port = htons(portNumber);
	m_serverAddress.sin_addr.s_addr = inetAddress;

	connectMessage.type = MSG_CONNECT;
	connectMessage.toId = ID_SERVER;
	connectMessage.fromId = ID_UNKNOWN;

	bytesSent = sendto(m_clientSocket, (char*)&connectMessage, sizeof(MSG_GENERIC_DATA), 0, (struct sockaddr*)&m_serverAddress, m_addressLength);

	if(bytesSent < 0)
	{
		return false;
	}

	startTime = timeGetTime();

	done = false;
	gotId = false;

	while(!done)
	{
		bytesRead = recvfrom(m_clientSocket, recvBuffer, 4096, 0, (struct sockaddr*)&m_serverAddress, &m_addressLength);

		if(bytesRead > 0)
		{
			done = true;
			gotId = true;
		}

		if(timeGetTime() > (startTime + 2000))
		{
			done = true;
			gotId = false;
		}
	}

	if(!gotId)
	{
		return false;
	}

	message = (MSG_GENERIC_DATA*)recvBuffer;

	if(message->type != MSG_ID)
	{
		return false;
	}

	if(message->toId == -1)
	{
		return false;
	}

	m_idNumber = message->toId;

	m_online = true;

	m_threadActive = false;

	threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)NetworkReadFunction, (void*)this, 0, &threadId);

	if(threadHandle == NULL)
	{
		return false;
	}

	m_latency = 0;
	m_pingTime = timeGetTime();

	return true;
}

void NetworkClass::SetThreadActive()
{
	m_threadActive = true;

	return;
}

void NetworkClass::SetThreadInactive()
{
	m_threadActive = false;

	return;
}

bool NetworkClass::Online()
{
	return m_online;
}

SOCKET NetworkClass::GetClientSocket()
{
	return m_clientSocket;
}

void NetworkClass::ReadNetworkMessage(char* recvBuffer)
{
	MSG_GENERIC_DATA* message;

	message = (MSG_GENERIC_DATA*)recvBuffer;

	switch(message->type)
	{
		case MSG_PING:
		{
			HandlePingMessage();
			break;
		}
		default:
		{
			break;
		}
	}

	return;
}

bool NetworkClass::InitializeWinSock()
{
	unsigned short version;
	WSADATA wsaData;
	int error;
	unsigned long bufferSize;
	WSAPROTOCOL_INFOW* protocolBuffer;
	int protocols[2];

	version = MAKEWORD(2, 0);

	error = WSAStartup(version, &wsaData);

	if(error != 0)
	{
		return false;
	}

	if((LOBYTE(wsaData.wVersion) != 2) || (HIBYTE(wsaData.wVersion) != 0))
	{
		return false;
	}

	WSAEnumProtocols(NULL, NULL, &bufferSize);

	protocolBuffer = new WSAPROTOCOL_INFOW[bufferSize];

	if(!protocolBuffer)
	{
		return false;
	}

	protocols[0] = IPPROTO_TCP;
	protocols[1] = IPPROTO_UDP;

	error = WSAEnumProtocols(protocols, protocolBuffer, &bufferSize);

	if(error == SOCKET_ERROR)
	{
		return false;
	}

	delete protocolBuffer;
	protocolBuffer = 0;

	return true;
}

void NetworkClass::ShutdownWinSock()
{
	WSACleanup();

	return;
}

void NetworkClass::ProcessLatency()
{
	if(timeGetTime() >= (m_pingTime + 5000))
	{
		m_pingTime = timeGetTime();
		SendPing();
	}
}

void NetworkClass::SendPing()
{
	MSG_GENERIC_DATA message;
	int bytesSent;

	message.type = MSG_PING;
	message.toId = ID_SERVER;
	message.fromId = m_idNumber;

	bytesSent = sendto(m_clientSocket, (char*)&message, sizeof(MSG_GENERIC_DATA), 0, (struct sockaddr*)&m_serverAddress, m_addressLength);

	if(bytesSent != sizeof(MSG_GENERIC_DATA))
	{

	}

	return;
}

void NetworkClass::HandlePingMessage()
{
	OutputDebugString(L"DA\n");
	m_latency = timeGetTime() - m_pingTime;
	return;
}

void NetworkClass::SendDisconnectMessage()
{
	MSG_GENERIC_DATA message;
	int bytesSent;

	message.type = MSG_DISCONNECT;
	message.toId = ID_SERVER;
	message.fromId = m_idNumber;

	bytesSent = sendto(m_clientSocket, (char*)&message, sizeof(MSG_GENERIC_DATA),0, (struct sockaddr*)&m_serverAddress, m_addressLength);

	if(bytesSent != sizeof(MSG_GENERIC_DATA))
	{

	}

	return;
}

void NetworkReadFunction(void* ptr)
{
	NetworkClass* networkClassPtr;
	struct sockaddr_in serverAddress;
	int addressLength;
	int bytesRead;
	char recvBuffer[4096];

	networkClassPtr = (NetworkClass*)ptr;

	networkClassPtr->SetThreadActive();

	addressLength = sizeof(serverAddress);

	while(networkClassPtr->Online())
	{
		bytesRead = recvfrom(networkClassPtr->GetClientSocket(), recvBuffer, 4096, 0, (struct sockaddr*)&serverAddress, &addressLength);

		if(bytesRead > 0)
		{
			networkClassPtr->ReadNetworkMessage(recvBuffer);
		}
	}

	networkClassPtr->SetThreadInactive();

	return;
}