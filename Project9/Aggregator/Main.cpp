#include "stdafx.h"
#include "Aggregator.h"

#define SLEEP_TIME_INTERVAL 20
#define INITIAL_SIZE 4

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

typedef struct structForhWaitForChilds
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
}T_StructForhWaitForChilds;

bool setNonblockingParams(SOCKET socket, bool isReceiving);
char* receive(int length, SOCKET socket);
void AddToArrayOfQueues(SOCKET socket, structForhWaitForChilds *tstruct);
int SendData(int size, char* data, SOCKET socket);
void ResizeArray(structForhWaitForChilds *tstructToBeHandled, size_t newCapacity);
void RemoveQueueFromTheArrayOfQueues(structForhWaitForChilds *tstructToBeHandled, size_t index);

//Thread's method that sends data from the queue
DWORD WINAPI Propagate(LPVOID lpParam)
{
	T_ArrayOfQueues *tarray = (T_ArrayOfQueues*)lpParam;

	while (true)
	{
		if (!tarray->isAlive)
		{
			printf("Closing thread...\n");

			CloseHandle(tarray->threadHandle);				//Can this be done here?

			return 0;
		}

		if (tarray->queue->GetCount() > 0)
		{
			T_StructForData retVal = tarray->queue->Dequeue();
			char *dataToSend = (char*)malloc(sizeof(char)*retVal.size + 4);
			*(int*)dataToSend = retVal.size;
			memcpy((char*)(dataToSend + sizeof(int)), retVal.data, retVal.size);
			SendData(retVal.size + 4, dataToSend, tarray->socket); //TODO: finish sending implementation
			Sleep(1);
		}
		else
		{
			Sleep(SLEEP_TIME_INTERVAL);
		}
	}

	return 0;
}

DWORD WINAPI receiveChilds(LPVOID lpParam)
{
	T_StructForhWaitForChilds *tstruct = (T_StructForhWaitForChilds*)lpParam;
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
		setNonblockingParams(someSocket2, true);
		//dodaj u niz
		AddToArrayOfQueues(accept(someSocket2, NULL, NULL), tstruct);
	}
	return 0;
}

DWORD WINAPI receiveDataFromParrent(LPVOID lpParam)
{
	T_StructForhWaitForChilds *tstruct = (T_StructForhWaitForChilds*)lpParam;
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

		char *lengthChar = (char*)malloc(sizeof(char) * 4);
		lengthChar = receive(4, tstruct->agr->GetConnectSocket());
		int length = *(int*)lengthChar;
		char *data = (char*)malloc(sizeof(char)*length);
		data = receive(length, tstruct->agr->GetConnectSocket());

		T_StructForData *dataForQueue = new T_StructForData();
		dataForQueue->size = length;
		dataForQueue->data = data;
		//TODO: UBACI DEO ZA PUNJENJE REDOVA!

		
		for(int i = 0; i < tstruct->count; i++)
		{
			tstruct->queues[i].queue->Enqueue(*dataForQueue);
		}
	} while (1);
}

bool setNonblockingParams(SOCKET socket, bool isReceiving)
{
	while(true)
	{
		int iResult = 0;
		// Initialize select parameters
		FD_SET set;
		timeval timeVal;

		FD_ZERO(&set);
		// Add socket we will wait to read from
		FD_SET(socket, &set);

		// Set timeouts to zero since we want select to return
		// instantaneously
		timeVal.tv_sec = 0;
		timeVal.tv_usec = 0;
		if(isReceiving)
		{
			iResult = select(0 /* ignored */, &set, NULL, NULL, &timeVal);
		}
		else
		{
			iResult = select(0 /* ignored */, NULL, &set, NULL, &timeVal);
		}
		// lets check if there was an error during select
		if (iResult == SOCKET_ERROR)
		{
			fprintf(stderr, "select failed with error: %ld\n", WSAGetLastError());
			return false;
		}
		if(iResult==0)
		{
			Sleep(500);
		}
		else
		{
			break;
		}
	}
		//NONBLOCKING SETTINGS END-----------------------------------------------------------
}

char* receive(int length, SOCKET socket)
{
	int received = 0;
	char* data = (char*)malloc(sizeof(char)*length);
	while(received<length)
	{
		setNonblockingParams(socket, true);
		received += recv(socket, data + received, length - received, 0);
		//TODO: hendlanje gresaka
		if (received == SOCKET_ERROR)
		{
			printf("Receive failed with error: %d\n", WSAGetLastError());
			closesocket(socket);
			WSACleanup();
		}
	}
	return data;
}

int SendData(int size, char* data, SOCKET socket)
{
	int iResult = 0;
	while (iResult < size)
	{
		setNonblockingParams(socket, false);
		iResult += send(socket, data + iResult, size, 0);
		if (iResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(socket);
			WSACleanup();
			return 1;
		}
	}
	return iResult;
}

void AddToArrayOfQueues(SOCKET socket, structForhWaitForChilds *tstruct)
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

void RemoveQueueFromTheArrayOfQueues(structForhWaitForChilds *tstructToBeHandled, size_t index)
{
	if (index < tstructToBeHandled->count && index >= 0)
	{
		delete(&(tstructToBeHandled->queues[index]));

		tstructToBeHandled->queues[index] = tstructToBeHandled->queues[tstructToBeHandled->count];
		tstructToBeHandled->count--;
	}
}

// Not thread safe! If used, it should be called inside a critical section.
void ResizeArray(structForhWaitForChilds *tstructToBeHandled, size_t newCapacity)
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
	T_StructForhWaitForChilds *tstruct = new T_StructForhWaitForChilds();
	tstruct->capacity = INITIAL_SIZE;
	tstruct->queues = (T_ArrayOfQueues*)malloc(sizeof(T_ArrayOfQueues)*INITIAL_SIZE);
	tstruct->agr = new Aggregator(port, ipAddress, portForChilds);
	InitializeCriticalSection(&(tstruct->criticalSection));
	DWORD itForChildsID;
	DWORD ithReceiveDataFromParrent;
	HANDLE hWaitForChilds = CreateThread(NULL, 0, &receiveChilds, tstruct, 0, &itForChildsID);
	HANDLE hReceiveDataFromParrent = CreateThread(NULL, 0, &receiveDataFromParrent, tstruct, 0, &ithReceiveDataFromParrent);
	int liI = getchar();
	liI = getchar();
	
	//Last changes in main
	for(int i =0; i<tstruct->count; i++)
	{
		tstruct->queues[i].isAlive = false;
		Sleep(1000);
		CloseHandle(tstruct->queues[i].threadHandle);
	}

	CloseHandle(hWaitForChilds);
	free(ipAddress);
	free(portForChilds);
	free(tstruct);
	WaitForSingleObject(hWaitForChilds, INFINITE);		
	closesocket(tstruct->agr->GetConnectSocket());
	WSACleanup();
	return 0;
}