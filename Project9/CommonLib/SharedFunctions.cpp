#include "SharedFunctions.h"

int dataReceived = 0;

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
}

bool SetNonblockingParams(SOCKET socket, bool isReceiving)
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