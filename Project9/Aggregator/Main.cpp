#include "stdafx.h"

int _tmain(int argc, _TCHAR* argv[])
{
	printf("Please enter the IP address to connect to: \n");
	char *ipAddress = (char*)malloc(sizeof(char));
	unsigned short port = 0;
	scanf("%s", ipAddress);
	printf("Pleas enter the port: \n");
	scanf("%us", &port);


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