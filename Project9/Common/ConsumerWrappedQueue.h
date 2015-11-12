#pragma once
#include "ThreadSafeQueue.h"

#define SLEEP_TIME_INTERVAL 5

template <class T>
class ConsumerWrappedQueue
{
public:
	ConsumerWrappedQueue();
	~ConsumerWrappedQueue()
	{
		CloseHandle(consumer);
	}

	void AddData(T newData)
	{
		queue->Enqueue(newData);
	}

	void SetShutDownThread(bool shutDown)
	{
		shutDownThread = shutDown;
	}

protected:
	ThreadSafeQueue<T> *queue = new ThreadSafeQueue<T>();
	DWORD producerID, consumerID;
	HANDLE consumer = CreateThread(NULL, 0, &print2, NULL, 0, &consumerID);
	bool shutDownThread = false;

	//Thread's method that returns element
	DWORD WINAPI GetData(LPVOID lpParam)
	{
		while (true) //DA LI JE OVO OK?
		{
			if (shutDownThread)
			{
				return 0;
			}
			
			if (queue->GetCount() > 0)
			{
				T retVal = queue->Dequeue();
			}
			else
			{
				Sleep(SLEEP_TIME_INTERVAL);
			}
		}
		return 0;
	}
};

