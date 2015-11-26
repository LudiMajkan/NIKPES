#include "stdafx.h"
#include "DataSource.h"
#include <time.h>
#include "main.h"
#include "../CommonLib/SharedFunctions.h"


DWORD WINAPI ReceiveChildren(LPVOID lpParam)
{
	T_StructForhWaitForChildren *tstruct = (T_StructForhWaitForChildren*)lpParam;
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
	bool socketCorrect = false;
	while (true)
	{
		if (tstruct->ShutdownThread)
		{
			return 0;
		}
		SOCKET someSocket2 = tstruct->ds->GetListenSocket();
		socketCorrect = SetNonblockingParams(someSocket2, true);
		/*if (!socketCorrect)
		{
			break;
		}*/
		//dodaj u niz
		AddToArrayOfSockets(accept(someSocket2, NULL, NULL), tstruct);
	}
	return 0;
}

/*bool SetNonblockingParams(SOCKET socket, bool isReceiving)
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
			iResult = select(0 /* ignored /, &set, NULL, NULL, &timeVal);
		}
		else
		{
			iResult = select(0 /* ignored /, NULL, &set, NULL, &timeVal);
		}
		// lets check if there was an error during select
		if (iResult == SOCKET_ERROR)
		{
			fprintf(stderr, "select failed with error: %ld\n", WSAGetLastError());
			return false;
		}
		if(iResult==0)
		{
			Sleep(SLEEP_TIME_INTERVAL);
		}
		else
		{
			break;
		}
	}
		//NONBLOCKING SETTINGS END-----------------------------------------------------------
	return true;
}*/

int SendData(int size, char* data, SOCKET socket)
{
	int iResult = 0;
	while(iResult < size)
	{
		bool socketCorrect = SetNonblockingParams(socket, false);
		if (!socketCorrect)
		{
			break;
		}
		iResult += send(socket, data + iResult, size, 0);
		if(iResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(socket);
			//WSACleanup();
			return 1;
		}
	}
	return iResult;
}

void AddToArrayOfSockets(SOCKET socket, T_StructForhWaitForChildren *tstruct)
{
	int count = tstruct->count;
	tstruct->sockets[count] = socket;
	tstruct->count++;
}

int _tmain(int argc, _TCHAR* argv[])
{
	
	char *port = (char*)malloc(sizeof(char)*5);
	printf("Pleas enter the port: \n");
	scanf("%s", port);
	DWORD itForChildsID;
	T_StructForhWaitForChildren  *tstruct = new T_StructForhWaitForChildren();
	tstruct->count = 0;
	tstruct->ShutdownThread = false;
	tstruct->ds = new DataSource(port);
	tstruct->sockets = (SOCKET*)malloc(sizeof(SOCKET)*10);
	//Inicijalizuj niz redova!
	HANDLE hWaitForChildren = CreateThread(NULL, 0, &ReceiveChildren, tstruct, 0, &itForChildsID);
	char liI = getchar();
	
	
	/*char *messageToSend = (char*)malloc(sizeof(char)*10);
	for(int i = 0; i<10; i++)
	{
			messageToSend[i] = 0x41;
	}*/

	char *messageToSend = (char*)malloc(sizeof(char)*1024);
	memset(messageToSend,'A', 1024);
	char *endMessage = (char*)malloc(sizeof(char)*1024);
	memset(endMessage, 'E', 1024);
	/*for(int i = 0; i<100000; i++)
	{
		//printf("trying to send data to aggregator\n");
		int sizeOfMessage = 1000008;
		int iResult = 0;
		iResult = SendData(sizeof(int), (char*)&sizeOfMessage, tstruct->sockets[0]);
		iResult = SendData(sizeof(char) * 1000008, messageToSend, tstruct->sockets[0]);
	}*/
	printf("Press enter to start sending data");
	liI = getchar();
	while(liI!='a')
	{
		time_t rawtime = time(0);
		struct tm *timeinfo;
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		printf(asctime(timeinfo));
		int iResult = 0;
		int sizeOfMessage = 1024;
		for(int i = 0; i<102399; i++)
		{
			iResult = 0;
			for(int j = 0; j< tstruct->count; j++)
			{
				iResult = SendData(sizeof(int), (char*)&sizeOfMessage, tstruct->sockets[j]);
				iResult = SendData(sizeof(char) * 1024, messageToSend, tstruct->sockets[j]);
				if (iResult == 0)
				{
					for (int k = j; k < tstruct->count; k++)
					{
						if (k + 1 < tstruct->count)
						{
							tstruct->sockets[k] = tstruct->sockets[k + 1];
						}
					}
					j--;
					tstruct->count--;
				}
			}
		}
		iResult = 0;
		iResult = SendData(sizeof(int), (char*)&sizeOfMessage, tstruct->sockets[0]);
		iResult = SendData(sizeof(char) * 1024, endMessage, tstruct->sockets[0]);
		printf("Sending done\n");
		liI = getchar(); 
	}

	liI = getchar();
	tstruct->ShutdownThread = true;
	Sleep(1);
	CloseHandle(hWaitForChildren);
	free(messageToSend);
	free(endMessage);
	tstruct->ds->~DataSource();
	WSACleanup();
	delete(tstruct);
	return 0;
}