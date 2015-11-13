#include "stdafx.h"
#include "Aggregator.h"

int _tmain(int argc, _TCHAR* argv[])
{
	printf("Please enter the IP address to connect to: \n");
	char *ipAddress = (char*)malloc(sizeof(char) * 16);
	unsigned short port = 0;
	scanf("%s", ipAddress);
	printf("Pleas enter the port for connecting to server: \n");
	scanf("%us", &port);
	char *portForChilds = (char*)malloc(sizeof(char) * 5);
	printf("please enter the port you want to listen: \n");
	scanf("%s", portForChilds);
	Aggregator *agr = new Aggregator(port, ipAddress, portForChilds);
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
	return 0;
}