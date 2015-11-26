#pragma once

#include "stdafx.h"
#include "DataDestination.h"
#include "../CommonLib/ThreadSafeQueue.h"

typedef struct structForData
{
	int size;
	char *data;
}T_StructForData;

typedef struct structForhWaitForChildren
{
	DataDestination *dd;
	SOCKET *socket;
	bool ShutdownThread;
	ThreadSafeQueue<T_StructForData> *queue;
}T_StructForhWaitForChildren;

/*bool SetNonblockingParams(SOCKET socket, bool isReceiving);
char* Receive(int length, SOCKET socket);*/