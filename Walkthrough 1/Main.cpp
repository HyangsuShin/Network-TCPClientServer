#include "iostream"
#include "NetworkManager.h"
#include <string>
#include <conio.h>
using namespace std;

int main()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	int colorCode = -1;
	int colorChoose = -1;
	cout << "Choose the color of the text you want to have: " << endl;
	cout << "\t1) Blue" << endl;
	cout << "\t2) Yellow" << endl;
	cout << "\t3) Red" << endl;
	cout << "\t4) Green" << endl;
	cin >> colorChoose;



	switch (colorChoose)
	{
	case 1:
		colorCode = FOREGROUND_BLUE;
		break;
	case 2:
		colorCode = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
		break;
	case 3:
		colorCode = FOREGROUND_RED;
		break;
	case 4:
		colorCode = FOREGROUND_GREEN;
		break;
	default:
		colorCode = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
		break;
	}
	SetConsoleTextAttribute(hConsole, colorCode);

	cout << "Helllo Networking World!" << endl;

	NetworkManager::GetInstance()->Init();
	NetworkManager::GetInstance()->CreateTCPSockets();
	
	const int SERVER = 1;
	cout << "Choose a role: " << endl;
	cout << "\t1) Server" << endl;
	cout << "\t2) Client" << endl;

	int choice = 0;
	cin >> choice;

	if (choice == SERVER)
	{
		NetworkManager::GetInstance()->BindTCP();
		NetworkManager::GetInstance()->ListenTCP();
	}
	else
	{
		NetworkManager::GetInstance()->ConnectTCP();
	}


	bool start = false;
	string myMsg;
	while (true)
	{
		if (choice == SERVER&& NetworkManager::GetInstance()->GetNumConnections() < 4)
		{
			NetworkManager::GetInstance()->AcceptConnectionsTCP();
		}
		if (_kbhit())
		{
			if (start)
			{
				cout << "Send: ";
			}
			start = true;
			getline(cin, myMsg);
			if (myMsg.length() > 0)
			{
				NetworkManager::GetInstance()->SendDataTCP(myMsg.c_str());
			}
		}
		
		char rcvMessage[NetworkManager::MAX_MSG_SIZE];
		int size = NetworkManager::GetInstance()->ReceiveDataTCP(rcvMessage);
		if (size > 0)
		{
			cout << "Received: " << rcvMessage << endl << endl;
			
			if (choice == SERVER)
			{
				NetworkManager::GetInstance()->SendDataTCP(rcvMessage);
			}
		}

	}
	

	NetworkManager::GetInstance()->ShutDown();	
	return 0;

}

