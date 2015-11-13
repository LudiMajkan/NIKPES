
#include "stdafx.h"
#include "DataSource.h"

typedef struct structForThread
{
	DataSource *ds;
	//niz redova
	bool ShutdownThread = false;
}T_StructForThread;

DWORD WINAPI receiveChilds(LPVOID lpParam)
{
	T_StructForThread *tstruct = (T_StructForThread*)lpParam;
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
	}
	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
	
	char *port = new char();
	printf("Pleas enter the port: \n");
	scanf("%s", port);
	DWORD itForChildsID;
	T_StructForThread  *tstruct = new T_StructForThread();
	tstruct->ds = new DataSource(port);
	//Inicijalizuj niz redova!
	HANDLE hWaitForChilds = CreateThread(NULL, 0, &receiveChilds, tstruct, 0, &itForChildsID);
	int liI = getchar();
	liI = getchar();

	CloseHandle(hWaitForChilds);

	WaitForSingleObject(hWaitForChilds, INFINITE);
	return 0;
}