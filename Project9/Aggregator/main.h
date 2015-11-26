#pragma once

#include "stdafx.h"
#include "Aggregator.h"
#include "../CommonLib/ThreadSafeQueue.h"

#define SLEEP_TIME_INTERVAL 20
#define INITIAL_SIZE 4
#define DEFAULT_BUFLEN 512



typedef struct structForData
{
	int size;
	char *data;
}T_StructForData;

typedef struct arrayOfQueues
{
	// A pointer to the ThreadSafeQueue object.
	ThreadSafeQueue<T_StructForData> *queue;

	// A HANDLE fot the consumer thread.
	HANDLE threadHandle;

	// A consumer thread ID
	DWORD threadID;

	// A socket used to connect to the client 
	SOCKET socket;

	// A flag that shows if the consumer thread should get terminated.
	bool isAlive;
}T_ArrayOfQueues;

typedef struct structForhWaitForChildren
{
	// Pointer to Aggregator that initializes connecion and server for children.
	Aggregator *agr;

	// The capacity of the "queues" array.
	int capacity;
	
	// The number of queues in the "queues" array.
	int count;

	// A dynamic array of queues with data.
	T_ArrayOfQueues *queues;

	// A flag that shows if the thread should get terminated.
	bool ShutdownThread;

	// A CRITICAL_SECTION object used for synchronization of threads.
	CRITICAL_SECTION criticalSection;
}T_StructForhWaitForChildren;

//bool SetNonblockingParams(SOCKET socket, bool isReceiving);
//char* Receive(int length, SOCKET socket);
void AddToArrayOfQueues(SOCKET socket, T_StructForhWaitForChildren *tstruct);
int SendData(int size, char* data, SOCKET socket);
void ResizeArray(T_StructForhWaitForChildren *tstructToBeHandled, size_t newCapacity);
void RemoveQueueFromTheArrayOfQueues(T_StructForhWaitForChildren *tstructToBeHandled, size_t index);
char* CreateDeepCopy(char* data,int length);