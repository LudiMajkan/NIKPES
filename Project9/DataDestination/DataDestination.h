/**
* @file DataDestination.h
*
* @brief Contains socket for getting children and socket for connecting to parrent
*
*/

/**
* @class DataDestination
* @brief Contains socket for connecting to parrent.
*
* This class initializes connection needed to connect to parent node.
*
*/

#pragma once
#include "stdafx.h"

#define DEFAULT_BUFLEN 512

class DataDestination
{
public:

	/**
	* @brief Constructor of DataDestination
	*/
	DataDestination(unsigned short port, char *ipAddress)
	{
		connectSocket = INVALID_SOCKET;
		InitializeConnection(ipAddress, port);
	}

	/**
	* @brief Destructor of DataDestination
	*/
	~DataDestination();

	/**
	* @brief Get access to connectSocket
	*
	*
	* @return Returns connectSocket
	*
	*/
	SOCKET GetConnectSocket()
	{
		return connectSocket;
	}

	/**
	* @brief Get access to serverAddress
	*
	* @return Returns serverAddress
	*
	*/
	sockaddr_in GetServerAddress()
	{
		return serverAddress;
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

	/**
	* @brief Initializes connection to parent node
	*
	* @param *ipAddress - pointer on ip address of server
	*
	* @param port - port of server application
	*
	* @return return value true if initialization is successfuly done,
	*	else returns false;
	*/
	bool InitializeConnection(char* ipAddress, unsigned short port);

	/**
	* @brief Initializes windows sockets
	*
	* @return return value true if initialization is successfuly done,
	*	else returns false;
	*/
	bool InitializeWindowsSockets();
};
