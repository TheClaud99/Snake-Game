#include "Winsock.h"
#include <stdio.h>


Socket::Socket()
{
	if(WSAStartup(MAKEWORD(2,2), &wsaData) != NO_ERROR)
	{
		printf("Error initialing WSA: %d", WSAGetLastError());
	}
	else 
	{
		if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
		{
			printf("%d", WSAGetLastError());
		}
	}
}

Socket::~Socket()
{
	freeSocket();
	WSACleanup();
}


void Socket::freeSocket()
{
	closesocket(ClientSocket);
	closesocket(ServerSocket);
	closesocket(AcceptSocket);
}

bool Socket::initServer(u_short port)
{
	bool success = true;
	char IP[32];
	getMyIp(IP);

	ServerService.sin_family = AF_INET;
	ServerService.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr(IP);
	ServerService.sin_port = htons(port);
	
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(ServerSocket == INVALID_SOCKET)
	{
		printf("Error creating socket: %d", WSAGetLastError(), "\n");
		success = false;
	}
	else
	{
		if(bind(ServerSocket, (SOCKADDR*) &ServerService, sizeof(ServerService)) == SOCKET_ERROR)
		{
			printf("Error binding socket with IP %s, error: %d", IP, WSAGetLastError(), "\n");
			success = false;
		}
		else
		{
			if(listen(ServerSocket, 1) == SOCKET_ERROR)
			{
				printf("Listenign error: %d", WSAGetLastError(), "\n");
				success = false;
			}
			else
			{
				printf("Waiting client connection on ip %s...\n", IP);
				do
				{	
					AcceptSocket = accept(ServerSocket, NULL, NULL);
				}
				while(AcceptSocket == SOCKET_ERROR);
		
				puts("Client connected");
			}
		}
	}
	return success;
}

int Socket::serverRecive(char message[], int size)
{
	int bytesRecv = recv(AcceptSocket, message, 32, 0);
	if(bytesRecv < 0)
	{
		printf("%d", WSAGetLastError());
	}
	
	return bytesRecv;
}

bool Socket::initClient(char IP[], u_short port)
{
	bool success = true;
	
	ClientService.sin_port = htons(port);
	ClientService.sin_family = AF_INET;
	ClientService.sin_addr.s_addr = inet_addr(IP);
	
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(ClientSocket == INVALID_SOCKET)
	{
		printf("%d", WSAGetLastError());
		success = false;
	}
	
	printf("Connecting with the server...\n");
	if(connect(ClientSocket, (SOCKADDR*) &ClientService, sizeof(ClientService)) == SOCKET_ERROR)
	{
		printf("Error connecting with the server, error: %d", WSAGetLastError());
		success = false;
	}
	
	return success;
}

void Socket::clientSend(char message[], int size)
{
	int bytesSent;

	bytesSent = send(ClientSocket, message, size, 0);
	if(bytesSent < 0)
	{
		printf("Errror sending data to the server, error: %d", WSAGetLastError());
	}
}

bool Socket::getMyIp(char IP[])
{
	char hBuffer[1024], tmp[4], dot[] = ".";
	bool success = true;
	int i;
	
	for(i = 0; i < 32; i++)
	{
		IP[i] = '\0';
	}
	
	
	if(gethostname(hBuffer, sizeof(hBuffer)) == SOCKET_ERROR)
	{
		printf("Error gettin local ip address: %d", WSAGetLastError());
		success = false;
	}
	else
	{
		hostent *host = gethostbyname(hBuffer);
		
		i =((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b1;
		
		sprintf(tmp, "%d", i);
		strcat(IP, tmp);		//add the 1st octet
		strcat(IP, dot);		//add the dot
		

		i = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b2;
		
		sprintf(tmp, "%d", i);
		strcat(IP, tmp);		//add the second octet
		strcat(IP, dot);		//add the dot
		
    	i = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b3;
    	
    	sprintf(tmp, "%d", i);
    	strcat(IP, tmp);		//add the third octet
    	strcat(IP, dot);		//add the dot
    	
    	i = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b4;
    	sprintf(tmp, "%d", i);
    	strcat(IP, tmp);		//add the fourth octet
 		
 		
	}
	
	return success;
}

void Socket::serverSend(char message[], int size)
{
	int bytesSent = send(AcceptSocket, message, size, 0);
	
	if(bytesSent < 0)
	{
		printf("Errror sending data to the client, error: %d", WSAGetLastError());
	}
}

int Socket::clientRecive(char message[], int size)
{
	int bytesRecv = recv(ClientSocket, message, size, 0);
	
	if(bytesRecv < 0)
	{
		printf ("%d", WSAGetLastError());
	}
	
	return bytesRecv;
}
