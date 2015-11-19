#include "stdafx.h"
#include "DataDestination.h"

typedef struct structForData
{
	int size;
	char *data;
}T_StructForData;

typedef struct structForhWaitForChilds
{
	DataDestination *dd;
	SOCKET *socket;
	bool ShutdownThread;
	ThreadSafeQueue<T_StructForData> *queue;
}T_StructForhWaitForChilds;

bool setNonblockingParams(SOCKET socket, bool isReceiving);
char* receive(int length, SOCKET socket);

DWORD WINAPI receiveDataFromParrent(LPVOID lpParam)
{
	T_StructForhWaitForChilds *tstruct = (T_StructForhWaitForChilds*)lpParam;
	tstruct->ShutdownThread = false;
	int iResult = 0;
	printf("DataDestination now receiving data...\n");
	//NONBLOCKING MODE
	unsigned long int nonBlockingMode = 1;
	iResult = ioctlsocket(tstruct->dd->GetConnectSocket(), FIONBIO, &nonBlockingMode);
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
		lengthChar = receive(4, tstruct->dd->GetConnectSocket());
		int length = *(int*)lengthChar;
		char *data = (char*)malloc(sizeof(char)*length);
		data = receive(length, tstruct->dd->GetConnectSocket());

		T_StructForData *dataForQueue = new T_StructForData();
		dataForQueue->size = length;
		dataForQueue->data = data;
		//TODO: UBACI DEO ZA PUNJENJE REDOVA!
		printf("Some data received:\n");
		for(int i = 0; i< length; i++)
		{
			printf("%c",data[i]);
		}
		printf("\n");
		tstruct->queue->Enqueue(*dataForQueue);
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
		setNonblockingParams(socket,true);
		received += recv(socket, data + received, length - received, 0);
	}
	return data;
}

int _tmain(int argc, _TCHAR* argv[])
{
	printf("Please enter the IP address to connect to: \n");
	unsigned short port = 0;
	char *ipAddress = (char*)malloc(sizeof(char) * 16);
	
	scanf("%s", ipAddress);
	printf("Pleas enter the port for connecting to server: \n");
	scanf("%hu", &port);
	T_StructForhWaitForChilds *tstruct = new T_StructForhWaitForChilds();
	tstruct->queue = new ThreadSafeQueue<T_StructForData>();
	tstruct->dd = new DataDestination(port, ipAddress);
	DWORD ithReceiveDataFromParrent;
	HANDLE hReceiveDataFromParrent = CreateThread(NULL, 0, &receiveDataFromParrent, tstruct, 0, &ithReceiveDataFromParrent);
	int liI = getchar();
	liI = getchar();

	tstruct->ShutdownThread = true;
	CloseHandle(hReceiveDataFromParrent);
	closesocket(tstruct->dd->GetConnectSocket());
	WSACleanup();
	return 0;
}