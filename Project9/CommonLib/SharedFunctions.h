/**
* @file SharedFunctions.h
*
* @brief Contains function that is used from other projects
*
*/
#pragma once

#include <stdio.h>
#include <ws2tcpip.h>
#include <WinSock2.h>

/**
* @brief Function for receiving data
*
* @param length - Number of bytes that should be received
*
* @param socket - Socket from which will data be received
*
* @return Returns received data
*/
char* Receive(int length, SOCKET socket);

/**
* @brief Setting non blocking params for socket
*
* @param socket - Socket that will be non blocking
*
* @param isReceiving - if socket is receiving data its true,
*		 else false
*
* @return Returns true if socket is set to non blocking,
*		  else false
*/
bool SetNonblockingParams(SOCKET socket, bool isReceiving);

/**
* @brief Function for seding data
*
* @param size - Number of bytes that should be sent
*
* @param socket - Data will be sent to this socket
*
* @return Returns number of sent data
*/
int SendData(int size, char* data, SOCKET socket);