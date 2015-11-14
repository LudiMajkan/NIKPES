#include "stdafx.h"
#include "DataDestination.h"

int _tmain(int argc, _TCHAR* argv[])
{
	printf("Please enter the IP address to connect to: \n");
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
	liI = getchar();
}