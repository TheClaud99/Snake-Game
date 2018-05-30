/*
This is the class wich implement
socket communication for the multiplayer
*/

#ifndef WINSOCK_H
#define WINSOCK_H

#include <winsock2.h>


class Socket
{
	public:
		
		Socket();
		
		~Socket();
		
		bool initServer(u_short port);
		
		int serverRecive(char message[], int size = 32);
		
		bool initClient(char IP[], u_short port);
		
		int clientRecive(char message[], int size = 32);
		
		void clientSend(char message[], int size = 32);
		
		void serverSend(char message[], int size = 32);
		
		void freeSocket();
		
		bool getMyIp(char IP[]);
		
	private:
		WSADATA wsaData;
		SOCKET ServerSocket;
		SOCKET ClientSocket;
		SOCKET AcceptSocket;
		sockaddr_in ServerService;
		sockaddr_in ClientService;
};

#endif
