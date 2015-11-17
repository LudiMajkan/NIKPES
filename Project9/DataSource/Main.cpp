
#include "stdafx.h"
#include "DataSource.h"

#define SLEEP_TIME_INTERVAL 20
#define TYPE_OF_DATA "char"

typedef struct arrayOfQueues
{
	ThreadSafeQueue<char> *queue;
	HANDLE threadHandle;
	DWORD threadID;
	SOCKET socket;
	bool isAlive;
}T_ArrayOfQueues;

typedef struct structForhWaitForChilds
{
	DataSource *ds;
	//SOCKET *sockets;//ZAMENITI DINAMICNIM NIZOM
	int count;
	arrayOfQueues *queues;
	bool ShutdownThread;
}T_StructForhWaitForChilds;

bool setNonblockingParams(SOCKET socket, bool isReceiving);
void AddToArrayOfQueues(SOCKET socket, structForhWaitForChilds *tstruct);

//Thread's method that sends data from the queue
DWORD WINAPI GetData(LPVOID lpParam)
{
	T_ArrayOfQueues *tarray = (T_ArrayOfQueues*)lpParam;

	while (true)
	{
		if (!tarray->isAlive)
		{
			printf("Closing thread...\n");

			CloseHandle(tarray->threadHandle);				//TODO: Can this be done here?

			return 0;
		}

		//ThreadSafeQueue<int> *queue = (array->queue);
		if (tarray->queue->GetCount() > 0)
		{
			int retVal = tarray->queue->Dequeue();
			printf("%d\n", retVal);
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
	iResult = ioctlsocket(tstruct->ds->GetListenSocket(), FIONBIO, &nonBlockingMode);
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
		SOCKET someSocket2 = tstruct->ds->GetListenSocket();
		setNonblockingParams(someSocket2, true);
		//dodaj u niz
		AddToArrayOfQueues(accept(someSocket2, NULL, NULL), tstruct);
	}
	return 0;
}

DWORD WINAPI sendDataToChilds(LPVOID lpParam)
{
	T_StructForhWaitForChilds *tstruct = (T_StructForhWaitForChilds*)lpParam;
	tstruct->ShutdownThread = false;
	int iResult = 0;
	//NONBLOCKING MODE
	unsigned long int nonBlockingMode = 1;
	iResult = ioctlsocket(tstruct->ds->GetListenSocket(), FIONBIO, &nonBlockingMode);
	//TODO: Implementation of sending (Maybe is not needed)?
	while(true)
	{
		if(tstruct->ShutdownThread)
		{
			return 0;
		}
		//setNonblockingParams(someSocket2, false);
		//for(int i=0; i<BROJ U NIZU; i++)
	}
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

int SendData(int size, char* data, SOCKET socket)
{
	int iResult = 0;
	while(iResult < size)
	{
		setNonblockingParams(socket, false);
		iResult += send(socket, data + iResult, size, 0);
		if(iResult == SOCKET_ERROR)
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
	//Critical sectrion?
	//TODO: Odradi prepakivanje u slucaju probijanja velicine (inicijalno 10)
	int count = tstruct->count;
	tstruct->queues[count].queue = new ThreadSafeQueue<char>();
	tstruct->queues[count].socket = socket;
	tstruct->queues[count].threadHandle = CreateThread(NULL, 0, &GetData, tstruct, 0, &tstruct->queues->threadID);
	tstruct->queues[count].isAlive = true;
	tstruct->count += 1;
}

int _tmain(int argc, _TCHAR* argv[])
{
	
	char *port = (char*)malloc(sizeof(char)*5);
	printf("Pleas enter the port: \n");
	scanf("%s", port);
	DWORD itForChildsID;
	DWORD itSendDataToChilds;
	T_StructForhWaitForChilds  *tstruct = new T_StructForhWaitForChilds();
	tstruct->count = 0;
	tstruct->ShutdownThread = false;
	tstruct->queues = (T_ArrayOfQueues*)malloc(sizeof(T_ArrayOfQueues)*10);
	//tstruct->sockets = (SOCKET*)malloc(sizeof(SOCKET)*10); //ZAMENITI DINAMICNIM NIZOM
	tstruct->ds = new DataSource(port);
	//Inicijalizuj niz redova!
	HANDLE hWaitForChilds = CreateThread(NULL, 0, &receiveChilds, tstruct, 0, &itForChildsID);
	HANDLE hSendDataToChilds = CreateThread(NULL, 0, &sendDataToChilds, tstruct, 0, &itSendDataToChilds);
	int liI = getchar();
	printf("Press enter to start sending data");
	liI = getchar();
	char *messageToSend = (char*)malloc(sizeof(char)*10);
	for(int i = 0; i<10; i++)
	{
			messageToSend[i] = 0x09;
	}
	
	for(int i = 0; i<10; i++)
	{
		printf("trying to send data to aggregator\n");
		int sizeOfMessage = 10;
		int iResult = 0;
		iResult = SendData(sizeof(int), (char*)&sizeOfMessage, tstruct->queues[0].socket);
		iResult = SendData(sizeof(char) * 10, messageToSend, tstruct->queues[0].socket);
	}
	liI = getchar(); 

	liI = getchar();

	CloseHandle(hWaitForChilds);

	WaitForSingleObject(hWaitForChilds, INFINITE);
	return 0;
}