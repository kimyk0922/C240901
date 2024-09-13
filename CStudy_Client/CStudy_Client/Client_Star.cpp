
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS


#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>

#include <Windows.h> //WSASelect, WSASend
#include <process.h>
#include <conio.h>

#pragma comment(lib, "ws2_32")

#pragma pack(push, 1)
struct Data
{
	int X;
	int Y;

};
#pragma pack(pop)

Data PlayerPosition;

CRITICAL_SECTION PlayerDataCS;


void Draw(Data Position)
{
	system("cls");
	COORD Cur;
	Cur.X = PlayerPosition.X;
	Cur.Y = PlayerPosition.Y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);

	printf("*");
}

//Worker Thread
//network
void RecvThread(void* Arg)
{
	Data Packet;
	char Buffer[1024] = { 0, };

	SOCKET ServerSocket = *(SOCKET*)(Arg);

	while (true)
	{
		int RecvByte = recv(ServerSocket, (char*)&Packet, sizeof(Packet), 0);

		if (RecvByte <= 0)
		{
			break;
		}

		EnterCriticalSection(&PlayerDataCS);
		PlayerPosition.X = ntohl(Packet.X);
		PlayerPosition.Y = ntohl(Packet.Y);
		LeaveCriticalSection(&PlayerDataCS);


		Draw(PlayerPosition);
	}
}


//main Thread
int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 0), &wsaData);

	InitializeCriticalSection(&PlayerDataCS);

	SOCKET ServerSocket;

	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in ServerSockAddr;
	ZeroMemory(&ServerSockAddr, 0);

	ServerSockAddr.sin_family = PF_INET;
	ServerSockAddr.sin_addr.s_addr = inet_addr("192.168.3.203");
	ServerSockAddr.sin_port = htons(3000);

	connect(ServerSocket, (struct sockaddr*)&ServerSockAddr, sizeof(ServerSockAddr));

	_beginthread(RecvThread, 0, (void*)&ServerSocket);


	while (true)
	{
		int KeyCode = _getch();

		EnterCriticalSection(&PlayerDataCS);
		switch (KeyCode)
		{
		case 'w':
			PlayerPosition.Y--;
			break;
		case 's':
			PlayerPosition.Y++;
			break;
		case 'a':
			PlayerPosition.X--;
			break;
		case 'd':
			PlayerPosition.X++;
			break;
		}

		Data Packet;

		Packet.X = htonl(PlayerPosition.X);
		Packet.Y = htonl(PlayerPosition.Y);


		int SendBytes = send(ServerSocket, (char*)&Packet, sizeof(Packet), 0);
		LeaveCriticalSection(&PlayerDataCS);

		Draw(PlayerPosition);
	}

	closesocket(ServerSocket);

	DeleteCriticalSection(&PlayerDataCS);

	WSACleanup();


	return 0;
}