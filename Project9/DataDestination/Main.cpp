#include "stdafx.h"
#include "DataDestination.h"
#include <time.h>
#include "main.h"
#include "../CommonLib/SharedFunctions.h"

extern int dataReceived;

DWORD WINAPI ReceiveDataFromParrent(LPVOID lpParam)
{
	T_StructForhWaitForChildren *tstruct = (T_StructForhWaitForChildren*)lpParam;
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

		char *lengthChar = Receive(4, tstruct->dd->GetConnectSocket());
		
		int length = *(int*)lengthChar;
		char *data = Receive(length, tstruct->dd->GetConnectSocket());

		if(data[0] == 'E')
		{
			time_t rawtime = time(0);
			struct tm *timeinfo;
			time ( &rawtime );
			timeinfo = localtime ( &rawtime );
			printf(asctime(timeinfo));
			//free(timeinfo);
		}

		free(data);
		free(lengthChar);
		//T_StructForData *dataForQueue = new T_StructForData();
		//dataForQueue->size = length;
		//dataForQueue->data = data;

		//tstruct->queue->Enqueue(*dataForQueue);
		dataReceived++;
	} while (1);
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
			if(dataReceived % 1000 >= 950)
			{
				printf("Data received: %d\n", dataReceived);
			}
			Sleep(1);
		}
		else
		{
			break;
		}
	}
		//NONBLOCKING SETTINGS END-----------------------------------------------------------
	return true;
}

char* Receive(int length, SOCKET socket)
{
	int received = 0;
	char* data = (char*)malloc(sizeof(char)*length);
	bool socketCorrect = false;
	while(received<length)
	{
		socketCorrect = SetNonblockingParams(socket, true);
		if (!socketCorrect)
		{
			break;
		}
		received += recv(socket, data + received, length - received, 0);
	}
	return data;
}*/

int _tmain(int argc, _TCHAR* argv[])
{
	printf("Please enter the IP address to connect to: \n");
	unsigned short port = 0;
	char *ipAddress = (char*)malloc(sizeof(char) * 16);
	
	scanf("%s", ipAddress);
	printf("Pleas enter the port for connecting to server: \n");
	scanf("%hu", &port);
	T_StructForhWaitForChildren *tstruct = new T_StructForhWaitForChildren();
	tstruct->queue = new ThreadSafeQueue<T_StructForData>();
	tstruct->dd = new DataDestination(port, ipAddress);
	DWORD ithReceiveDataFromParrent;
	HANDLE hReceiveDataFromParrent = CreateThread(NULL, 0, &ReceiveDataFromParrent, tstruct, 0, &ithReceiveDataFromParrent);
	int liI = getchar();
	liI = getchar();

	tstruct->ShutdownThread = true;
	CloseHandle(hReceiveDataFromParrent);
	tstruct->dd->~DataDestination();
	free(tstruct->queue);
	WSACleanup();
	return 0;
}