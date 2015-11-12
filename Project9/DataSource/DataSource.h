#pragma once

#include "stdafx.h"
#include "DataSource.h"

#define DEFAULT_BUFLEN 512

class DataSource
{
public:
	DataSource(char* port);
	~DataSource();
	SOCKET GetListenSocket()
	{
		return listenSocket;
	}
	SOCKET GetAcceptedSocket()
	{
		return acceptedSocket;
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
	// Socket used for listening for new clients 
	SOCKET listenSocket = INVALID_SOCKET;
	// Socket used for communication with client
	SOCKET acceptedSocket = INVALID_SOCKET;
	int iResult;
	// Buffer used for storing incoming data
	char recvbuf[DEFAULT_BUFLEN];
	// Prepare address information structures
	addrinfo *resultingAddress = NULL;
	addrinfo hints;

	bool InitializeServer(char *port);
	bool InitializeWindowsSockets();

};