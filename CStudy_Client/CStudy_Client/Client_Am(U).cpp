#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS


#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>

#include <Windows.h> //WSASelect, WSASend
#include <process.h>

#pragma comment(lib, "ws2_32")


//Worker Thread
//network
void RecvThread(void* Arg)
{
	char Buffer[1024] = { 0, };

	SOCKET ServerSocket = *(SOCKET*)(Arg);

	while (true)
	{
		int RecvByte = recv(ServerSocket, Buffer, 1024, 0);

		if (RecvByte <= 0)
		{
			break;
		}


		printf("%s", Buffer);
	}
}


//main Thread
int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 0), &wsaData);

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
		char Message[1024] = { 0, };

		//input, process
		scanf("%s", Message);
		int SendBytes = send(ServerSocket, Message, (int)strlen(Message), 0);

	}

	closesocket(ServerSocket);


	WSACleanup();


	return 0;
}