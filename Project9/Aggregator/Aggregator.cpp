// Aggregator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{

	ThreadSafeQueue<int> *q = new ThreadSafeQueue<int>();

	q->Enqueue(1);
	q->Enqueue(2);
	q->Enqueue(3);
	q->Enqueue(4);

	int a = q->Dequeue();
	int b = q->Dequeue();
	int c = q->Dequeue();
	q->Enqueue(5);
	q->Enqueue(6);
	a = q->Dequeue();
	b = q->Dequeue();
	c = q->Dequeue();
	q->~ThreadSafeQueue();
	return 0;
}

