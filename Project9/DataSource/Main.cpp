
#include "stdafx.h"
#include "DataSource.h"

typedef struct structForhWaitForChilds
{
	DataSource *ds;
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
		SOCKET someSocket2 = tstruct->ds->GetListenSocket();
		SOCKET someSocket = accept(someSocket2, NULL, NULL);
		tstruct->ds->SetAcceptedSocket(someSocket);
		//dodaj u niz
	}
	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
	
	char *port = (char*)malloc(sizeof(char)*5);
	printf("Pleas enter the port: \n");
	scanf("%s", port);
	DWORD itForChildsID;
	T_StructForhWaitForChilds  *tstruct = new T_StructForhWaitForChilds();
	tstruct->ds = new DataSource(port);
	//Inicijalizuj niz redova!
	HANDLE hWaitForChilds = CreateThread(NULL, 0, &receiveChilds, tstruct, 0, &itForChildsID);
	int liI = getchar();
	printf("Press enter to start sending data");
	liI = getchar();

	char *messageToSend = "hello aggergator!";
	printf("try to send data to aggregator");
	int iResult = send(tstruct->ds->GetAcceptedSocket(), messageToSend, (int)strlen(messageToSend) + 1, 0);

	liI = getchar(); 
	liI = getchar();

	CloseHandle(hWaitForChilds);

	WaitForSingleObject(hWaitForChilds, INFINITE);
	return 0;
}