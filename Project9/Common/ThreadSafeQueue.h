#pragma once

#include "stdafx.h"

#define INITIAL_SIZE 4
#define QUEUE_IS_EMPTY -2147483648

template <class T>
class ThreadSafeQueue
{
public:
	//Not needed
	ThreadSafeQueue(T* dataToCopy, int size)
	{
		head = 0;
		tail = -1;
		capacity = INITIAL_SIZE;
		InitializeCriticalSection(&criticalSection);
		count = size;
		if (newCapacity >= capacity)
		{
			capacity = newCapacity;
		}

		data = new T[capacity];
		for (int i = 0; i < newCapacity; i++)
		{
			Enqueue(dataToCopy[i]);
		}
	}
	ThreadSafeQueue(int initialSize)
	{
		head = 0;
		tail = -1;
		InitializeCriticalSection(&criticalSection);
		count = 0;
		data = new T[initialSize];
		capacity = initialSize;
	}
	ThreadSafeQueue()
	{
		head = 0;
		tail = -1;
		InitializeCriticalSection(&criticalSection);
		capacity = INITIAL_SIZE;
		data = new T[INITIAL_SIZE];
		count = 0;
	}

	~ThreadSafeQueue()
	{
		DeleteCriticalSection(&criticalSection);

		delete(data);
	}

	int GetCount()
	{
		//TODO: When there is 2 aggrs access violation occurs
		EnterCriticalSection(&criticalSection);
		int retVal = count;
		LeaveCriticalSection(&criticalSection);
		return retVal;
		
	}

	void Enqueue(T newData)
	{
		EnterCriticalSection(&criticalSection);

		tail = (tail + 1) % capacity;

		if (tail == head && count > 0)
		{
			Resize(capacity * 2);
		}

		data[tail] = newData;
		count++;
		LeaveCriticalSection(&criticalSection);
	}

	T Dequeue()
	{
		EnterCriticalSection(&criticalSection);
		T retVal;
		/*if (count == 0)
		{
			return QUEUE_IS_EMPTY;
		}*/
		if (count > 0)
		{
			retVal = data[head];
			if (count > 1)
			{
				head = (head + 1) % capacity;
			}

			if (head < tail)
			{
				if (count - 1 <= capacity / 4)
				{
					Resize(capacity / 2);
				}
			}
			else if (head > tail)
			{
				if (count - 1 <= capacity / 4) // PROVERI OVO!!!!!
				{
					Resize(capacity / 2);
				}
			}
			else 
			{
				if (count - 1 <= capacity / 4)
				{
					Resize(capacity / 2);
				}
			}
			count--;
		}
		LeaveCriticalSection(&criticalSection);
		return retVal;
	}

protected:
	T *data;
	int count;
	int capacity;
	int head;
	int tail;

	CRITICAL_SECTION criticalSection;

	void Resize(int newCapacity)
	{
		if (newCapacity >= INITIAL_SIZE)
		{
			T *newData = new T[newCapacity];
			if (head > tail)
			{
				for (int i = head; i < capacity; i++)
				{
					newData[i - head] = data[i];
				}

				for (int i = 0; i <= tail; i++)
				{
					newData[capacity - head + i] = data[i];
				}

				// Postavljamo tail na stari capacity koji je zapravo broj elemenata. proveri ovo!
				tail = count;
			}
			else if (head < tail)
			{
				for (int i = head; i <= tail; i++)
				{
					newData[i - head] = data[i];
				}

				tail = count;
			}
			else
			{
				int dataIndex = head;
				newData[0] = data[dataIndex];
				for (int i = 1; i < count; i++)
				{
					dataIndex = (dataIndex + 1) % capacity;
					newData[i] = data[dataIndex];
				}
				tail = count;
			}
			capacity = newCapacity;
			head = 0;

			delete(data);
			data = newData;
		}
	}


};

