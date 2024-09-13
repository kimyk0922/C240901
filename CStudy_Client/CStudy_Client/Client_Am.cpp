#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS


#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>

#include <Windows.h> //WSASelect, WSASend

#pragma comment(lib, "ws2_32")

int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 0), &wsaData);

	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in ServerSockAddr;
	ZeroMemory(&ServerSockAddr, 0);

	ServerSockAddr.sin_family = PF_INET;
	ServerSockAddr.sin_addr.s_addr = inet_addr("192.168.3.182");
	ServerSockAddr.sin_port = htons(3000);

	connect(ServerSocket, (struct sockaddr*)&ServerSockAddr, sizeof(ServerSockAddr));


	while (true)
	{
		char Message[1024] = { 0, };
		scanf("%s", Message);

		int SendBytes = send(ServerSocket, Message, (int)strlen(Message), 0);

		char Buffer[1024] = { 0, };
		int RecvByte = recv(ServerSocket, Buffer, 1024, 0);

		printf("%s", Buffer);
	}

	closesocket(ServerSocket);


	WSACleanup();


	return 0;
}