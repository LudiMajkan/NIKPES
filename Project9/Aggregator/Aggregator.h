#pragma once

#include "stdafx.h"

#define DEFAULT_BUFLEN 512

class Aggregator
{
public:
	Aggregator(unsigned short port, char *ipAddress, char *portForChilds)
	{
		connectSocket = INVALID_SOCKET;
		listenSocket = INVALID_SOCKET;
		acceptedSocket = INVALID_SOCKET;
		resultingAddress = NULL;

		InitializeConnection(ipAddress, port);
		InitializeServer(portForChilds);
	}
	~Aggregator()
	{
		closesocket(connectSocket);
		closesocket(acceptedSocket);
		closesocket(listenSocket);
	}
	SOCKET GetConnectSocket()
	{
		return connectSocket;
	}
	int GetIResult()
	{
		return iResult;
	}
	sockaddr_in GetServerAddress()
	{
		return serverAddress;
	}
	SOCKET GetListenSocket()
	{
		return listenSocket;
	}
	SOCKET GetAcceptedSocket()
	{
		return acceptedSocket;
	}
	void SetAcceptedSocket(SOCKET acceptedSocketNew)
	{
		acceptedSocket = acceptedSocketNew;
	}
	char* GetRecvbuf()
	{
		return recvbuf;
	}
	addrinfo *GetResultingAddress()
	{
		return resultingAddress;
	}

	addrinfo GetHints()
	{
		return hints;
	}
protected:
	// socket used to communicate with server
	SOCKET connectSocket;
	// variable used to store function return value
	int iResult;
	// create and initialize address structure
	sockaddr_in serverAddress;

	//SERVER PARAMETERS
	// Socket used for listening for new clients 
	SOCKET listenSocket;
	// Socket used for communication with client
	SOCKET acceptedSocket;
	//int iResult;
	// Buffer used for storing incoming data
	char recvbuf[DEFAULT_BUFLEN];
	// Prepare address information structures
	addrinfo *resultingAddress;
	addrinfo hints;

	bool InitializeServer(char *port);
	bool InitializeConnection(char* ipAddress, unsigned short port);
	bool InitializeWindowsSockets();
	
};