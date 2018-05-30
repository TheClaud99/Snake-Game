#include "Socket.h"
#include <stdio.h>

Socket::~Socket()
{
	freeSocket();
}


void Socket::freeSocket()
{
	close(ClientSocket);
	close(ServerSocket);
	close(AcceptSocket);
}

bool Socket::initServer(u_short port)
{
	bool success = true;
	char IP[] = "127.0.0.1";

	ServerService.sin_family = AF_INET;
	ServerService.sin_addr.s_addr = inet_addr(INADDR_ANY);
	ServerService.sin_port = htons(port);
	
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(ServerSocket == -1)
	{
		printf("Error creating socket: %s", strerror(errno), "\n");
		success = false;
	}
	else
	{
		if(bind(ServerSocket, (sockaddr*) &ServerService, sizeof(ServerService)) == -1)
		{
			printf("Error binding socket with IP %s, error: ", strerror(errno), "\n");
			success = false;
		}
		else
		{
			if(listen(ServerSocket, 1) == -1)
			{
				printf("Listenign error: %s",strerror(errno), "\n");
				success = false;
			}
			else
			{
				printf("Waiting client connection...\n");
				do
				{	
					AcceptSocket = accept(ServerSocket, NULL, NULL);
				}
				while(AcceptSocket == -1);
		
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
		printf("%d", strerror(errno));
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
	if(ClientSocket == -1)
	{
		printf("%d", strerror(errno));
		success = false;
	}
	
	printf("Connecting with the server...\n");
	if(connect(ClientSocket, (sockaddr*) &ClientService, sizeof(ClientService)) == -1)
	{
		printf("Error connecting with the server, error: %d", strerror(errno));
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
		printf("Errror sending data to the server, error: %d", strerror(errno));
	}
}


void Socket::serverSend(char message[], int size)
{
	int bytesSent = send(AcceptSocket, message, size, 0);
	
	if(bytesSent < 0)
	{
		printf("Errror sending data to the client, error: %d", strerror(errno));
	}
}

int Socket::clientRecive(char message[], int size)
{
	int bytesRecv = recv(ClientSocket, message, size, 0);
	
	if(bytesRecv < 0)
	{
		printf ("%d", strerror(errno));
	}
	
	return bytesRecv;
}


bool Socket::getMyIp(char IP[])
{
	//only works on Windows
	return true;
}