#ifndef WINSOCK_H
#define WINSOCK_H
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


class Socket
{
	public:
		
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
		int ServerSocket;
		int ClientSocket;
		int AcceptSocket;
		sockaddr_in ServerService;
		sockaddr_in ClientService;
};

#endif
