#include "stdafx.h"

class Aggregator
{
public:
	Aggregator(unsigned short port, char *ipAddress);
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
	bool InitializeConnection(char* ipAddress, unsigned short port);
	bool InitializeWindowsSockets();
};