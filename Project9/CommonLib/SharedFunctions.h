#pragma once

//#include "stdafx.h"

//#include <ws2tcpip.h>
#include <stdio.h>
#include <ws2tcpip.h>
#include <WinSock2.h>

char* Receive(int length, SOCKET socket);
bool SetNonblockingParams(SOCKET socket, bool isReceiving);
