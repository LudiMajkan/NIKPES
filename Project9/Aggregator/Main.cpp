#include "stdafx.h"
#include "Aggregator.h"

typedef struct structForhWaitForChilds
{
	Aggregator *agr;
	//niz redova
	bool ShutdownThread = false;
}T_StructForhWaitForChilds;

DWORD WINAPI receiveChilds(LPVOID lpParam)
{
	T_StructForhWaitForChilds *tstruct = (T_StructForhWaitForChilds*)lpParam;
	int iResult = 0;
	while (true)
	{
		if (tstruct->ShutdownThread)
		{
			return 0;
		}
		SOCKET someSocket2 = tstruct->agr->GetListenSocket();
		SOCKET someSocket = accept(someSocket2, NULL, NULL);
		tstruct->agr->SetAcceptedSocket(someSocket);
	}
	return 0;
}

DWORD WINAPI receiveDataFromParrent(LPVOID lpParam)
{
	T_StructForhWaitForChilds *tstruct = (T_StructForhWaitForChilds*)lpParam;
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
	// Initialize select parameters
	FD_SET set;
	timeval timeVal;

	FD_ZERO(&set);
	// Add socket we will wait to read from
	FD_SET(tstruct->agr->GetConnectSocket(), &set);

	// Set timeouts to zero since we want select to return
	// instantaneously
	timeVal.tv_sec = 0;
	timeVal.tv_usec = 0;

	iResult = select(0 /* ignored */, &set, NULL, NULL, &timeVal);

	// lets check if there was an error during select
	if (iResult == SOCKET_ERROR)
	{
		fprintf(stderr, "select failed with error: %ld\n", WSAGetLastError());
		return false;
	}
	//NONBLOCKING SETTINGS END-----------------------------------------------------------
	do
	{
		if (tstruct->ShutdownThread)
		{
			return 0;
		}
		iResult = recv(tstruct->agr->GetConnectSocket(), tstruct->agr->GetRecvbuf(), DEFAULT_BUFLEN, 0);
		if (iResult > 0)
		{
			printf("received data: %s\n", tstruct->agr->GetRecvbuf());
			printf("try to send data to Aggregator or DataSource\n");
			if (tstruct->agr->GetAcceptedSocket() != INVALID_SOCKET)
				iResult = send(tstruct->agr->GetAcceptedSocket(), tstruct->agr->GetRecvbuf(), (int)strlen(tstruct->agr->GetRecvbuf()) + 1, 0);
		}
		else
		{
			printf("Have no job to do, let's sleep a little \n");
			Sleep(3000);
		}
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
	char *portForChilds = (char*)malloc(sizeof(char) * 6);
	printf("please enter the port you want to listen: \n");
	scanf("%s", portForChilds);	
	T_StructForhWaitForChilds *tstruct = new T_StructForhWaitForChilds();
	tstruct->agr = new Aggregator(port, ipAddress, portForChilds);
	DWORD itForChildsID;
	DWORD ithReceiveDataFromParrent;
	HANDLE hWaitForChilds = CreateThread(NULL, 0, &receiveChilds, tstruct, 0, &itForChildsID);
	HANDLE hReceiveDataFromParrent = CreateThread(NULL, 0, &receiveDataFromParrent, tstruct, 0, &ithReceiveDataFromParrent);
	int liI = getchar();
	liI = getchar();
	//iResult = send(connectSocket, messageToSend, (int)strlen(messageToSend) + 1, 0);													

	/*ThreadSafeQueue<int> *q = new ThreadSafeQueue<int>();

	q->Enqueue(1);
	q->Enqueue(2);
	q->Enqueue(3);
	q->Enqueue(4);
	q->Enqueue(5);
	int a = q->Dequeue();
	int b = q->Dequeue();
	int c = q->Dequeue();
	a = q->Dequeue();
	b = q->Dequeue();
	q->~ThreadSafeQueue();

	q->Enqueue(0);
	int a = q->Dequeue();
	int b = q->Dequeue();*/

	CloseHandle(hWaitForChilds);
	free(ipAddress);
	free(portForChilds);
	free(tstruct);
	WaitForSingleObject(hWaitForChilds, INFINITE);																						
	return 0;
}