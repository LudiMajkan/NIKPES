// Aggregator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{

	ThreadSafeQueue<int> *q = new ThreadSafeQueue<int>();

	q->Enqueue(0x43434343);
	//q->Enqueue(0x91919191);
	q->Dequeue();
	//q->Dequeue();


	return 0;
}

