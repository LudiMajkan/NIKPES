/**
* @file DataSource.h
*
* @brief Contains socket for getting children and sockets for sending data to child nodes
*
*/

/**
* @class DataSource
* @brief Contains socket for getting children and sockets for sending data to child nodes
*
* This class initializes connection needed for child nodes to connect and sends data to child nodes
*
*/

#pragma once

#include "stdafx.h"
#include "DataSource.h"

#define DEFAULT_BUFLEN 512

class DataSource
{
public:

	/**
	* @brief Constructor of DataSource
	*/
	DataSource(char* port);

	/**
	* @brief Destructor of DataSource
	*/
	~DataSource();

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
	// Socket used for listening for new clients 
	SOCKET listenSocket;
	// Socket used for communication with client
	SOCKET acceptedSocket;
	int iResult;
	// Buffer used for storing incoming data
	char recvbuf[DEFAULT_BUFLEN];
	// Prepare address information structures
	addrinfo *resultingAddress;
	addrinfo hints;

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
	bool InitializeServer(char *port);

	/**
	* @brief Initializes windows sockets
	*
	* @return return value true if initialization is successfuly done,
	*	else returns false;
	*/
	bool InitializeWindowsSockets();

};