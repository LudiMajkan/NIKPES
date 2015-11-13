#pragma once

#include "stdafx.h"

#define DEFAULT_BUFLEN 512

class Aggregator
{
public:
	Aggregator(unsigned short port, char *ipAddress, char *portForChilds)
	{
		InitializeConnection(ipAddress, port);
		InitializeServer(portForChilds);
	}
	~Aggregator();
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
protected:
	// socket used to communicate with server
	SOCKET connectSocket = INVALID_SOCKET;
	// variable used to store function return value
	int iResult;
	// create and initialize address structure
	sockaddr_in serverAddress;

	//SERVER PARAMETERS
	// Socket used for listening for new clients 
	SOCKET listenSocket = INVALID_SOCKET;
	// Socket used for communication with client
	SOCKET acceptedSocket = INVALID_SOCKET;
	//int iResult;
	// Buffer used for storing incoming data
	char recvbuf[DEFAULT_BUFLEN];
	// Prepare address information structures
	addrinfo *resultingAddress = NULL;
	addrinfo hints;

	bool InitializeServer(char *port);
	bool InitializeConnection(char* ipAddress, unsigned short port);
	bool InitializeWindowsSockets();
	
};