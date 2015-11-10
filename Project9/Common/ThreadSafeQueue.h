#pragma once

#include "stdafx.h"

#define INITIAL_SIZE 100

template <class T>
class ThreadSafeQueue
{
protected:
	T *data;
	int count = 0;
	int capacity = INITIAL_SIZE;
	int head = 0;
	int tail = -1;

	CRITICAL_SECTION criticalSection;
	
	void Resize(int newCapacity)
	{
		if (newCapacity > INITIAL_SIZE)
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

				// Postavljamo tail na stari capacity koji je zapravo broj elemenata.
				tail = capacity;
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
				newData[0] = data[head];
			}
			capacity = newCapacity;
			head = 0;

			delete(data);
			data = newData;
		}
	}


public:
	ThreadSafeQueue(T* dataToCopy, int size)
	{
		InitializeCriticalSection(&criticalSection);

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
		InitializeCriticalSection(&criticalSection);

		data = new T[initialSize];
		capacity = initialSize;
	}
	ThreadSafeQueue()
	{
		InitializeCriticalSection(&criticalSection);

		data = new T[INITIAL_SIZE];
	}

	~ThreadSafeQueue()
	{
		DeleteCriticalSection(&criticalSection);

		delete(data);
	}

	void Enqueue(T newData)
	{
		EnterCriticalSection(&criticalSection);
		
		tail = (tail + 1) % capacity;
		// Brljavi pri prvom dodavanju...
		if (tail == head)
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

		T retVal = data[head];
		if (count > 1)
		{
			head = (head + 1) % capacity;
		}

		if (head < tail)
		{
			if (tail - head + 1 <= capacity / 4)
			{
				Resize(capacity / 2);
			}
		}
		else if (head > tail)
		{
			if (capacity - head + tail + 1 <= capacity / 4) // PROVERI OVO!!!!!
			{
				Resize(capacity / 2);
			}
		}
		else
		{
			// Ovo je samo za probavanje koda...
			//cout << "Queue is empty";
		}
		count--;
		LeaveCriticalSection(&criticalSection);

		return retVal;
	}
	//void Clear();

};

