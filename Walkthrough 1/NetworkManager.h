#pragma once
#include "iostream"

#pragma comment (lib, "ws2_32.lib")

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>

using namespace std;
class NetworkManager
{
public:
	static NetworkManager* GetInstance()
	{
		if (instance == nullptr)
		{
			instance = new NetworkManager();
		}
		return instance;
	}

	void Init();
	void ShutDown();
	void CreateUDPSockets();
	void CreateTCPSockets();
	void BindUDP();
	void BindTCP();
	void SetRemoteData();
	void SendDataUDP(const char* message);
	void SendDataTCP(const char* message);

	int ReceiveDataUDP(char* message);
	int ReceiveDataTCP(char* message);
	void ListenTCP();
	void ConnectTCP();
	void AcceptConnectionsTCP();
	
	int GetNumConnections()
	{
		return numConnections;
	}

	static const int MAX_MSG_SIZE = 1500;

	int rcvCheck = -1;


private:
	NetworkManager();
	~NetworkManager();

	static NetworkManager* instance;

	SOCKET UDPSocketIn;
	SOCKET UDPSocketOut;

	SOCKET TCPSocketIn;
	SOCKET TCPSocketOut[4];

	SOCKADDR_IN UDPOutAddr;
	SOCKADDR_IN UDPInAddr;

	SOCKADDR_IN TCPOutAddr[4];
	SOCKADDR_IN TCPInAddr;

	int numConnections = 0;
};

