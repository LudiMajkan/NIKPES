/**
* @file Propagator.h
*
* @brief Contains socket for getting children and socket for connecting to parrent
*
*/

/**
* @class Propagator
* @brief Contains socket for getting children and socket for connecting to parrent.
*
* This class initializes connection needed for child nodes to connect and connection needed to connect to parent node.
*
*/

#pragma once

#include "stdafx.h"
#define DEFAULT_BUFLEN 512
class Aggregator
{
public:

	/**
	* @brief Constructor of Aggregator
	*/
	Aggregator(unsigned short port, char *ipAddress, char *portForChilds)
	{
		connectSocket = INVALID_SOCKET;
		listenSocket = INVALID_SOCKET;
		acceptedSocket = INVALID_SOCKET;
		resultingAddress = NULL;

		InitializeConnection(ipAddress, port);
		InitializeServer(portForChilds);
	}

	/**
	* @brief Destructor of Aggregator
	*/
	~Aggregator()
	{
		closesocket(connectSocket);
		closesocket(acceptedSocket);
		closesocket(listenSocket);
	}

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

	/**
	* @brief Get access to listenSocket
	*
	*
	* @return Returns listenSocket
	*
	*/
	SOCKET GetListenSocket()
	{
		return listenSocket;
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

	/**
	* @brief Initializes server for accepting children
	*
	* @param *port - pointer on port that is used for listenSocket
	*
	* @return return value true if initialization is successfuly done,
	*	else returns false;
	*/
	bool InitializeServer(char *port);

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