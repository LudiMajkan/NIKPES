#pragma once
#include "stdafx.h"

#define DEFAULT_BUFLEN 512

class DataDestination
{
public:
	DataDestination(unsigned short port, char *ipAddress)
	{
		connectSocket = INVALID_SOCKET;
		InitializeConnection(ipAddress, port);
	}
	~DataDestination();
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
	char* GetRecvbuf()
	{
		return recvbuf;
	}
protected:
	// socket used to communicate with server
	SOCKET connectSocket;
	// variable used to store function return value
	int iResult;
	// create and initialize address structure
	sockaddr_in serverAddress;
	// Buffer used for storing incoming data
	char recvbuf[DEFAULT_BUFLEN];

	bool InitializeServer(char *port);
	bool InitializeConnection(char* ipAddress, unsigned short port);
	bool InitializeWindowsSockets();
};
