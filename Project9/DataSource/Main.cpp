#include "stdafx.h"
#include "DataSource.h"
int _tmain(int argc, _TCHAR* argv[])
{
	char *port = new char();
	printf("Pleas enter the port: \n");
	scanf("%s", port);
	DataSource *ds = new DataSource(port);
	return 0;
}