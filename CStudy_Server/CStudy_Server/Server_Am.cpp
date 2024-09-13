#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <iostream>


#pragma comment (lib, "ws2_32")



int main()
{

	WSAData wsaData;

	WSAStartup(MAKEWORD(2, 0), &wsaData);

	SOCKET ListenSocket;
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN ServerSockAddr;
	memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));

	ServerSockAddr.sin_family = PF_INET;
	inet_pton(AF_INET, "192.168.3.182", (PVOID) & (ServerSockAddr.sin_addr.s_addr));

	ServerSockAddr.sin_port = htons(3000);

//	AF_INET6 == IPv6 , PVOID == VOID POINTER

	bind(ListenSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));

	listen(ListenSocket, 0);

	fd_set ReadSockets;
	fd_set CopyReadSockets;

	FD_ZERO(&ReadSockets);					//	�ʱ�ȭ ����
	FD_ZERO(&CopyReadSockets);				//	�ʱ�ȭ ����

	FD_SET(ListenSocket, &ReadSockets);		//	���� ����


	timeval TimeOut;
	TimeOut.tv_sec = 0;
	TimeOut.tv_usec = 0;

	//	fd_set *writefds : ���ñ��
	//	fd_set *exceptfds : ����ó��


	while (true)
	{

		CopyReadSockets = ReadSockets;

		int ChangedSocketCount = select(0, &CopyReadSockets, 0, 0, &TimeOut);

		if (ChangedSocketCount == 0)
		{

			//������ �ٸ� �۾�
//			printf("processing");
			continue;		//	�ݺ��� �ȿ��� ����Ѵ�.

		}

		//	�������� 
		if (ChangedSocketCount < 0)
		{

			printf("error %d", GetLastError());
			exit(-1);

		}

		for (int FDIndex = 0; FDIndex < (int)ReadSockets.fd_count; ++FDIndex)
		{

			if (FD_ISSET(ReadSockets.fd_array[FDIndex], &CopyReadSockets))
			{

				if (ReadSockets.fd_array[FDIndex] == ListenSocket)
				{

				//	accept
					SOCKADDR_IN ClientSockAddr;
					memset(&ClientSockAddr, 0, sizeof(ClientSockAddr));
					int ClientSocketLength = sizeof(ClientSockAddr);
					SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSockAddr, &ClientSocketLength);
					FD_SET(ClientSocket, &ReadSockets);
					printf("connect client %s", inet_ntoa(ClientSockAddr.sin_addr));
	

				}

				else
				{

					char Buffer[1024] = { 0, };
					int RecvBytes = recv(ReadSockets.fd_array[FDIndex], Buffer, 1024, 0);
					if (RecvBytes <= 0)
					{

						SOCKET DisconnectedSocket = ReadSockets.fd_array[FDIndex];
						FD_CLR(DisconnectedSocket, &ReadSockets);

						//����
						printf("disconnect");

						closesocket(DisconnectedSocket);

					}
					else
					{

						for (int FDIndex2 = 0; FDIndex2 < ReadSockets.fd_count; ++FDIndex2)
						{

							if (ReadSockets.fd_array[FDIndex2] != ListenSocket)
							{

								send(ReadSockets.fd_array[FDIndex2], Buffer, (int)strlen(Buffer), 0);


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