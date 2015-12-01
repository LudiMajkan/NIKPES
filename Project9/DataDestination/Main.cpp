#include "stdafx.h"
#include "DataDestination.h"
#include <time.h>
#include "main.h"
#include "../CommonLib/SharedFunctions.h"

extern int dataReceived;

/**
* @brief Thread's function that receive data from parrent node
*
*/
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

		if(data[0] == 'E' && data!=NULL)
		{
			time_t rawtime = time(0);
			struct tm *timeinfo;
			time ( &rawtime );
			timeinfo = localtime ( &rawtime );
			printf(asctime(timeinfo));
		}

		free(data);
		free(lengthChar);
		dataReceived++;
	} while (1);
}

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