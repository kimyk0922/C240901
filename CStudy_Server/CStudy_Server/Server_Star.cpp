#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>


#pragma comment(lib, "ws2_32")

#pragma pack(push, 1)
struct Data
{
	int X;
	int Y;

};
#pragma pack(pop)

Data PlayerPosition;

void Draw(Data Position)
{
	system("cls");
	COORD Cur;
	Cur.X = PlayerPosition.X;
	Cur.Y = PlayerPosition.Y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);

	printf("*");
}


void SendData(SOCKET RecvSocket, Data Position)
{
	Data Packet;

	Packet.X = htonl(Position.X);
	Packet.Y = htonl(Position.Y);

	send(RecvSocket, (char*)&Packet, sizeof(Packet), 0);
}

int main()
{
	WSAData wsaData;

	WSAStartup(MAKEWORD(2, 0), &wsaData);

	SOCKET ListenSocket;
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN ServerSockAddr;
	memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));
	ServerSockAddr.sin_family = PF_INET;
	inet_pton(AF_INET,
		"192.168.3.203",
		(PVOID) & (ServerSockAddr.sin_addr.s_addr)
	);
	ServerSockAddr.sin_port = htons(3000);

	bind(ListenSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));

	listen(ListenSocket, 0);

	fd_set ReadSockets;
	fd_set CopyReadSockets;
	FD_ZERO(&ReadSockets);
	FD_ZERO(&CopyReadSockets);

	FD_SET(ListenSocket, &ReadSockets);

	timeval TimeOut;
	TimeOut.tv_sec = 0;
	TimeOut.tv_usec = 10;

	PlayerPosition.X = 10;
	PlayerPosition.Y = 10;

	while (true)
	{
		CopyReadSockets = ReadSockets;

		int ChangedSocketCount = select(0, &CopyReadSockets, 0, 0, &TimeOut);

		if (ChangedSocketCount == 0)
		{
			//다른작업
			//printf("processing\n");
			continue;
		}

		//서버한테 자료가 오면 소켓 네트워크 프로그래밍
		if (ChangedSocketCount < 0)
		{
			printf("error %d", GetLastError());
			exit(-1);
		}

		for (int i = 0; i < (int)ReadSockets.fd_count; ++i)
		{
			if (FD_ISSET(ReadSockets.fd_array[i], &CopyReadSockets))
			{
				if (ReadSockets.fd_array[i] == ListenSocket)
				{
					//accept
					SOCKADDR_IN ClientSockAddr;
					memset(&ClientSockAddr, 0, sizeof(ClientSockAddr));
					int ClinetSocketLength = sizeof(ClientSockAddr);
					SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSockAddr, &ClinetSocketLength);
					FD_SET(ClientSocket, &ReadSockets);
					printf("connect client %s\n", inet_ntoa(ClientSockAddr.sin_addr));
					SendData(ClientSocket, PlayerPosition);
				}
				else
				{
					Data Packet;

					int RecvBytes = recv(ReadSockets.fd_array[i], (char*)&Packet, sizeof(Packet), 0);

					PlayerPosition.X = ntohl(Packet.X);
					PlayerPosition.Y = ntohl(Packet.Y);

					Draw(PlayerPosition);

					SOCKADDR_IN ClientSockAddr;
					memset(&ClientSockAddr, 0, sizeof(ClientSockAddr));
					int ClinetSocketLength = sizeof(ClientSockAddr);

					getpeername(ReadSockets.fd_array[i], (struct sockaddr*)&ClientSockAddr, &ClinetSocketLength);

					if (RecvBytes <= 0)
					{
						SOCKET DisconnectedSocket = ReadSockets.fd_array[i];
						FD_CLR(DisconnectedSocket, &ReadSockets);
						//게임 
						printf("disconnect %s\n", inet_ntoa(ClientSockAddr.sin_addr));
						closesocket(DisconnectedSocket);
					}
					else
					{
						for (int j = 0; j < (int)ReadSockets.fd_count; ++j)
						{
							if (ReadSockets.fd_array[j] != ListenSocket)
							{
								//send(ReadSockets.fd_array[j], Buffer, (int)strlen(Buffer), 0);
								SendData(ReadSockets.fd_array[j], PlayerPosition);
							}
						}
					}
				}
			}
		}

	}



	WSACleanup();


	return 0;
}