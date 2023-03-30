#include "NetworkManager.h"
#include "WS2tcpip.h"


NetworkManager* NetworkManager::instance = nullptr;
NetworkManager::NetworkManager()
{}
NetworkManager::~NetworkManager()
{}

void NetworkManager::Init()
{
	cout << "NetworkManager init" << endl;

	WSADATA lpWSAData;
	int error = WSAStartup(MAKEWORD(2, 2), &lpWSAData);

	if (error != 0)
	{
		cout << "WSA failed with error: " << WSAGetLastError() << endl;
	}
}

void NetworkManager::ShutDown()
{
	cout << "NetworkManager::ShutDown() called." << endl;
	int error = WSAGetLastError(); //call this func once at the very beginning of the shutdown func

	if (error != 0)
	{
		cout << "WSA failed with error: " << error << endl;
	}

	if (UDPSocketIn != INVALID_SOCKET)
	{
		if (closesocket(UDPSocketIn) != 0)
		{
			cout << "Error! Closing UDPSocket in!!" << endl;
		}
	}

	if (UDPSocketOut != INVALID_SOCKET)
	{
		if (closesocket(UDPSocketOut) != 0)
		{
			cout << "Error! Closing UDPSocket out!!" << endl;
		}
	}

	if (TCPSocketIn != INVALID_SOCKET)
	{
		if (closesocket(TCPSocketIn) != 0)
		{
			cout << "Error! Closing TCPSocket in!!" << endl;
		}
	}

	if (TCPSocketOut[numConnections] != INVALID_SOCKET)
	{
		if (closesocket(TCPSocketOut[numConnections]) != 0)
		{
			cout << "Error! Closing TCPSocket out!!" << endl;
		}
	}
	WSACleanup();
	exit(0);
}



void NetworkManager::BindTCP()
{
	//using IPV4
	TCPInAddr.sin_family = AF_INET;
	TCPInAddr.sin_port = htons(8889);
	TCPInAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int bindError = bind(TCPSocketIn, reinterpret_cast<SOCKADDR*>(&TCPInAddr), sizeof(TCPInAddr));
	if (bindError == SOCKET_ERROR)
	{
		cout << "[ERROR] binding TCP in Socket" << endl;
	}
}

void NetworkManager::ListenTCP()
{
	listen(TCPSocketIn, SOMAXCONN);
}

void NetworkManager::ConnectTCP()
{
	TCPOutAddr[numConnections].sin_family = AF_INET;
	TCPOutAddr[numConnections].sin_port = htons(8889);
	string ip;
	cout << "Please enter the receiptient's IP: "; //Major #1
	cin >> ip;
	inet_pton(AF_INET, ip.c_str(), &TCPOutAddr[numConnections].sin_addr);
	//inet_pton(AF_INET, "127.0.0.1", &TCPOutAddr[numConnections].sin_addr);
	int connectStatus = connect(TCPSocketOut[numConnections], reinterpret_cast<sockaddr*>(&TCPOutAddr[numConnections]), sizeof(TCPOutAddr[numConnections]));;

	
	if (connectStatus == SOCKET_ERROR)
	{
		cout << "Error! Clients Connnection through TCP failed" << endl;
		ShutDown();
	}

	numConnections++;
	unsigned long unblocking = 1;
	ioctlsocket(TCPSocketOut[numConnections-1], FIONBIO, &unblocking);
}

void NetworkManager::AcceptConnectionsTCP()
{
	int clientSize = sizeof(TCPOutAddr[numConnections]);

	TCPSocketOut[numConnections] = accept(TCPSocketIn, reinterpret_cast<sockaddr*>(&TCPOutAddr[numConnections]), &clientSize);

	if (TCPSocketOut[numConnections] != INVALID_SOCKET)
	{
		char ipConnected[32];
		inet_ntop(AF_INET, &TCPOutAddr[numConnections].sin_addr, ipConnected, 32);
		cout << "User with ip: " << ipConnected << " just connected!" << endl;
		numConnections++;
		cout << "We have " << NetworkManager::GetInstance()->GetNumConnections() << " connection" << endl;

	}

	//switch to unblocking
	unsigned long unblocking = 1;
	ioctlsocket(TCPSocketOut[numConnections-1], FIONBIO, &unblocking);
	ioctlsocket(TCPSocketIn, FIONBIO, &unblocking);
}





void NetworkManager::CreateTCPSockets()
{
	cout << "NetworkManager::CreateTCPSockets()" << endl;

	//i.AF_INET: tell it we are using IPv4 
	//ii. SOCK_DGRAM: we are using UDP 
	//iii. IPPROTO_UDP: Any Protocol Available
	TCPSocketIn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (TCPSocketIn == INVALID_SOCKET)
	{
		cout << "TCP socket in failed to create!" << endl;
		ShutDown();
	}

	TCPSocketOut[numConnections] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (TCPSocketOut[numConnections] == INVALID_SOCKET)
	{
		cout << "TCP socket out failed to create!" << endl;
		ShutDown();
	}
}

void NetworkManager::SendDataTCP(const char* message)
{
	for (int i = 0; i < numConnections; i++)
	{
		if (i != rcvCheck)
		{
			int totalBytesSent = send(TCPSocketOut[i], message, strlen(message) + 1, 0);
			if (totalBytesSent == SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				if (error == WSAEWOULDBLOCK)
				{
					cout << "sent: " << totalBytesSent << " of data. " << endl;



				}
				else
				{
					cout << "Error! TCP Send Failed!" << endl;
					ShutDown();
				}
			}
		}
	}




}





int NetworkManager::ReceiveDataTCP(char* message)
{
	int byteReceived = 0;
	rcvCheck = -1;

	for (int i = 0; i < numConnections; i++)
	{
		byteReceived = recv(TCPSocketOut[i], message, MAX_MSG_SIZE, 0);

		if (byteReceived == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			if (error != WSAEWOULDBLOCK)
			{
				cout << "Error! error receiving from TCP " << endl;
				ShutDown();
			}
		}
		if (byteReceived > 0)
		{
			rcvCheck = i;
			return byteReceived;
		}
	}

	return byteReceived;
}

void NetworkManager::BindUDP()
{
	//using IPV4
	UDPInAddr.sin_family = AF_INET;
	UDPInAddr.sin_port = htons(8889);
	UDPInAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int bindError = bind(UDPSocketIn, reinterpret_cast<SOCKADDR*>(&UDPInAddr), sizeof(UDPInAddr));
	if (bindError == SOCKET_ERROR)
	{
		cout << "[ERROR] binding UDP in Socket" << endl;
	}
}

void NetworkManager::SetRemoteData()
{
	UDPOutAddr.sin_family = AF_INET;
	UDPOutAddr.sin_port = htons(8889);
	inet_pton(AF_INET, "127.0.0.1", &UDPOutAddr.sin_addr);
}
void NetworkManager::SendDataUDP(const char* message)
{
	int totalBytesSent = sendto(UDPSocketOut, message, strlen(message) + 1, 0,
		reinterpret_cast<SOCKADDR*>(&UDPOutAddr), sizeof(UDPOutAddr));

	if (totalBytesSent == SOCKET_ERROR)
	{
		ShutDown();
	}

	cout << "sent: " << totalBytesSent << " of data. " << endl;
}
int NetworkManager::ReceiveDataUDP(char* message)
{
	int byteReceived = 0;
	int inAddrSize = sizeof(UDPInAddr);

	byteReceived = recvfrom(UDPSocketIn, message, MAX_MSG_SIZE, 0, reinterpret_cast<SOCKADDR*>(&UDPInAddr), &inAddrSize);

	if (byteReceived == SOCKET_ERROR)
	{
		ShutDown();
	}

	return byteReceived;
}

void NetworkManager::CreateUDPSockets()
{
	cout << "NetworkManager::CreateUDPSockets()" << endl;

	//i.AF_INET: tell it we are using IPv4 
	//ii. SOCK_DGRAM: we are using UDP 
	//iii. IPPROTO_UDP: Any Protocol Available
	UDPSocketIn = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (UDPSocketIn == INVALID_SOCKET)
	{
		cout << "UDP socket in failed to create!" << endl;
		ShutDown();
	}

	UDPSocketOut = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (UDPSocketIn == INVALID_SOCKET)
	{
		cout << "UDP socket out failed to create!" << endl;
		ShutDown();
	}
}
