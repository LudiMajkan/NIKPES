#pragma once

#include "stdafx.h"
#include "DataSource.h"

#define SLEEP_TIME_INTERVAL 1

typedef struct structForhWaitForChildren
{
	DataSource *ds;
	SOCKET *sockets;
	int count;
	bool ShutdownThread;
}T_StructForhWaitForChildren;

//bool SetNonblockingParams(SOCKET socket, bool isReceiving);
void AddToArrayOfSockets(SOCKET socket, T_StructForhWaitForChildren *tstruct);
