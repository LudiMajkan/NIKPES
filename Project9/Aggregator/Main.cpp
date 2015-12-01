#include "stdafx.h"
#include "Aggregator.h"
#include "main.h"
#include "../CommonLib/SharedFunctions.h"

extern int dataReceived;

/**
* @brief Thread's method that sends data from the queue
*
*/
DWORD WINAPI Propagate(LPVOID lpParam)
{
	T_ArrayOfQueues *tarray = (T_ArrayOfQueues*)lpParam;

	while (true)
	{
		if (!tarray->isAlive)
		{
			printf("Closing thread...\n");
			return 0;
		}

		if (tarray->queue->GetCount() > 0)
		{
			T_StructForData *retVal = tarray->queue->Dequeue();
			char *dataToSend = (char*)malloc(sizeof(char)*retVal->size + 4);
			*(int*)dataToSend = retVal->size;
			memcpy((char*)(dataToSend + sizeof(int)), retVal->data, retVal->size);
			int sendingError = SendData(retVal->size + 4, dataToSend, tarray->socket); 
			if (sendingError == -1)
			{
				tarray->queue = NULL;
				free(dataToSend);
				free(retVal->data);
				free(retVal);
				return 0;
			}
			free(dataToSend);
			free(retVal->data);
			free(retVal);
		}
		else
		{
			Sleep(SLEEP_TIME_INTERVAL);
		}
	}

	return 0;
}

/**
* @brief Thread's function that accepts child nodes that connect to parrent
*
*/
DWORD WINAPI ReceiveChildren(LPVOID lpParam)
{
	T_StructForhWaitForChildren *tstruct = (T_StructForhWaitForChildren*)lpParam;
	tstruct->ShutdownThread = false;
	int iResult = 0;
	//NONBLOCKING MODE
	unsigned long int nonBlockingMode = 1;
	iResult = ioctlsocket(tstruct->agr->GetListenSocket(), FIONBIO, &nonBlockingMode);
	if (iResult == SOCKET_ERROR)
	{
		printf("ioctlsocket failed with error: %ld\n", WSAGetLastError());
		return 1;
	}
	
	while (true)
	{
		if (tstruct->ShutdownThread)
		{
			return 0;
		}
		SOCKET someSocket2 = tstruct->agr->GetListenSocket();
		SetNonblockingParams(someSocket2, true);
		AddToArrayOfQueues(accept(someSocket2, NULL, NULL), tstruct);
	}
	return 0;
}

/**
* @brief Thread's function that receive data from parrent node
*
*/
DWORD WINAPI ReceiveDataFromParrent(LPVOID lpParam)
{
	T_StructForhWaitForChildren *tstruct = (T_StructForhWaitForChildren*)lpParam;
	tstruct->ShutdownThread = false;
	int iResult = 0;
	
	printf("Aggregator now receiving data...\n");
	//NONBLOCKING MODE
	unsigned long int nonBlockingMode = 1;
	iResult = ioctlsocket(tstruct->agr->GetConnectSocket(), FIONBIO, &nonBlockingMode);
	if (iResult == SOCKET_ERROR)
	{
		printf("ioctlsocket failed with error: %ld\n", WSAGetLastError());
		return 1;
	}
	do
	{
		if (tstruct->ShutdownThread)
		{
			return 0;
		}

		char *lengthChar = Receive(4, tstruct->agr->GetConnectSocket());
		if(tstruct->count > 0)
		{
			int length = *(int*)lengthChar;
			char *data = Receive(length, tstruct->agr->GetConnectSocket());

			T_StructForData *dataForQueue = new T_StructForData();
			


			for(int i = 0; i < tstruct->count; i++)
			{
				if (tstruct->queues[i].queue != NULL)
				{
					char *copyData = CreateDeepCopy(data, length);
					dataForQueue->size = length;
					dataForQueue->data = copyData;
					tstruct->queues[i].queue->Enqueue(*dataForQueue);
					dataReceived++;
				}
			}
			free(data);
			free(dataForQueue);
		}
		free(lengthChar);
		
	} while (true);
}

/**
* @brief Creates deep copy 
*
* @param *data - pointer on data that will be copied
*
* @param length - length of data
*
* @return return new copy of data if data is not NULL,
*		  else returns NULL
*/
char* CreateDeepCopy(char* data,int length)
{
	if (data != NULL)
	{
		char *newCopy = (char *)malloc(sizeof(char) * length);
		memcpy(newCopy, data, length);
		return newCopy;
	}
	else
	{
		return NULL;
	}
}

/**
* @brief Adds element to array
*
* @param socket - socket for connection to child node
*
* @param *tstruct - Pointer on structure that contains array
*
*/
void AddToArrayOfQueues(SOCKET socket, T_StructForhWaitForChildren *tstruct)
{
	EnterCriticalSection(&(tstruct->criticalSection));

	int count = tstruct->count;
	if (count == tstruct->capacity)
	{
		ResizeArray(tstruct, tstruct->capacity * 2);
	}

	tstruct->queues[count].queue = new ThreadSafeQueue<T_StructForData>();
	tstruct->queues[count].socket = socket;
	tstruct->queues[count].threadHandle = CreateThread(NULL, 0, &Propagate, &(tstruct->queues[count]), 0, &tstruct->queues->threadID);
	tstruct->queues[count].isAlive = true;
	tstruct->count++;

	LeaveCriticalSection(&(tstruct->criticalSection));
}

/**
* @brief Removes element from array
*
* @param *tstructToBeHandled - Pointer on structure that contains array
*
* @param index - Index of element for deletion
*
*/
void RemoveQueueFromTheArrayOfQueues(T_StructForhWaitForChildren *tstructToBeHandled, size_t index)
{
	EnterCriticalSection(&(tstructToBeHandled->criticalSection));
	if (index < tstructToBeHandled->count && index >= 0)
	{
		delete(&(tstructToBeHandled->queues[index]));

		tstructToBeHandled->queues[index] = tstructToBeHandled->queues[tstructToBeHandled->count];
		tstructToBeHandled->count--;
	}
	LeaveCriticalSection(&(tstructToBeHandled->criticalSection));
}

// Not thread safe! If used, it should be called inside a critical section.
/**
* @brief Resizes the array
*
* @param *tstructToBeHandled - Pointer on structure that contains array
*
* @param newCapacity - Specifies new capacity of array
*
* IMPORTANT: Not thread safe! If used, it should be called inside a critical section!
*/
void ResizeArray(T_StructForhWaitForChildren *tstructToBeHandled, size_t newCapacity)
{
	if (newCapacity > INITIAL_SIZE)
	{
		arrayOfQueues *newData = new arrayOfQueues[newCapacity];

		// Copy the data from old array to the newly created array...
		memcpy(newData, tstructToBeHandled->queues, sizeof(T_ArrayOfQueues) * tstructToBeHandled->count);

		tstructToBeHandled->capacity = newCapacity;
		
		delete(tstructToBeHandled->queues);
		tstructToBeHandled->queues = newData;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	printf("Please enter the IP address to connect to: \n");
	unsigned short port = 0;
	char *ipAddress = (char*)malloc(sizeof(char) * 16);
	
	scanf("%s", ipAddress);
	printf("Pleas enter the port for connecting to server: \n");
	scanf("%hu", &port);
	char *portForChilds = (char*)malloc(sizeof(char) * 6);
	printf("please enter the port you want to listen: \n");
	scanf("%s", portForChilds);	
	T_StructForhWaitForChildren *tstruct = new T_StructForhWaitForChildren();
	tstruct->capacity = INITIAL_SIZE;
	tstruct->queues = (T_ArrayOfQueues*)malloc(sizeof(T_ArrayOfQueues)*INITIAL_SIZE);
	tstruct->agr = new Aggregator(port, ipAddress, portForChilds);
	InitializeCriticalSection(&(tstruct->criticalSection));
	DWORD itForChildsID;
	DWORD ithReceiveDataFromParrent;
	HANDLE hWaitForChildren = CreateThread(NULL, 0, &ReceiveChildren, tstruct, 0, &itForChildsID);
	HANDLE hReceiveDataFromParrent = CreateThread(NULL, 0, &ReceiveDataFromParrent, tstruct, 0, &ithReceiveDataFromParrent);
	int liI = getchar();
	liI = getchar();

	for(int i =0; i<tstruct->count; i++)
	{
		tstruct->queues[i].isAlive = false;
		Sleep(1000);
		CloseHandle(tstruct->queues[i].threadHandle);
	}

	CloseHandle(hWaitForChildren);
	CloseHandle(hReceiveDataFromParrent);
	free(ipAddress);
	free(portForChilds);
	WaitForSingleObject(hWaitForChildren, INFINITE);		
	tstruct->agr->~Aggregator();
	WSACleanup();
	delete(tstruct);
	return 0;
}