#include "stdafx.h"
#include "DataDestination.h"

typedef struct structForhWaitForChilds
{
	DataDestination *dd;
	SOCKET *sockets;//ZAMENITI DINAMICNIM NIZOM
	bool ShutdownThread;
}T_StructForhWaitForChilds;

bool setNonblockingParams(SOCKET socket, bool isReceiving);
char* receive(int length, SOCKET socket);

DWORD WINAPI receiveDataFromParrent(LPVOID lpParam)
{
	T_StructForhWaitForChilds *tstruct = (T_StructForhWaitForChilds*)lpParam;
	tstruct->ShutdownThread = false;
	int iResult = 0;
	printf("Aggregator now receiving data...\n");
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

		setNonblockingParams(tstruct->dd->GetConnectSocket(),true);
		char *lengthChar = receive(4, tstruct->dd->GetConnectSocket());
		int length = *(int*)lengthChar;
		setNonblockingParams(tstruct->dd->GetConnectSocket(),true);
		char *data = receive(length, tstruct->dd->GetConnectSocket());
		printf("received data: %s\n", tstruct->dd->GetRecvbuf());
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
		received += recv(socket, data + received, length - received, 0);
	}
	return data;
}

int _tmain(int argc, _TCHAR* argv[])
{
	/*printf("Please enter the IP address to connect to: \n");
	unsigned short port = 0;
	char *ipAddress = (char*)malloc(sizeof(char) * 16);

	scanf("%s", ipAddress);
	printf("Pleas enter the port for connecting to server: \n");
	scanf("%hu", &port);
	DataDestination *agr = new DataDestination(port, ipAddress);
	int liI = getchar();
	printf("Press enter to start receiving data");
	liI = getchar();

	int iResult = 0;
	printf("DataDestination now receiving data...\n");
	iResult = recv(agr->GetConnectSocket(), agr->GetRecvbuf(), DEFAULT_BUFLEN, 0);

	printf("received data: %s\n", agr->GetRecvbuf());
	liI = getchar();*/

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
	tstruct->dd = new DataDestination(port, ipAddress);
	DWORD ithReceiveDataFromParrent;
	HANDLE hReceiveDataFromParrent = CreateThread(NULL, 0, &receiveDataFromParrent, tstruct, 0, &ithReceiveDataFromParrent);
	int liI = getchar();
	liI = getchar();
}