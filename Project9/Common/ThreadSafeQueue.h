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

		data = static_cast< T* >( calloc(size, sizeof(T)) );
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
		data = static_cast< T* >( calloc(initialSize, sizeof(T)) );
		capacity = initialSize;
	}
	ThreadSafeQueue()
	{
		head = 0;
		tail = -1;
		InitializeCriticalSection(&criticalSection);
		capacity = INITIAL_SIZE;
		data = static_cast< T* >( calloc(INITIAL_SIZE, sizeof(T)) );
		count = 0;
	}

	~ThreadSafeQueue()
	{
		DeleteCriticalSection(&criticalSection);

		free(data);
	}

	int GetCount()
	{
		EnterCriticalSection(&criticalSection);
		int retVal = count;
		LeaveCriticalSection(&criticalSection);
		return retVal;
	}

	void Enqueue(T newData)
	{
		EnterCriticalSection(&criticalSection);

		if (count + 1 >= capacity)
		{
			Resize(capacity * 2);
		}

		tail = (tail + 1) % capacity;
		data[tail] = newData;
		count++;

		LeaveCriticalSection(&criticalSection);
	}

	T Dequeue()
	{
		EnterCriticalSection(&criticalSection);
		T retVal;

		if (count > 0)
		{
			retVal = data[head];
			head = (head + 1) % capacity;
			count--;
			
			if (count <= capacity / 4) // PROVERI OVO!!!!!
			{
				Resize(capacity / 2);
			}
			
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
			int brojIf = 0;
			char *temp = (char*)malloc(5);
			memset(temp,1,5);
			free(temp);
			T *newData =  static_cast< T* >( calloc(newCapacity,sizeof(T)) );
			if (head > tail)
			{
				brojIf = 1;
				for (int i = head; i < capacity; i++)
				{
					newData[i - head] = data[i];
				}

				for (int i = 0; i <= tail; i++)
				{
					newData[capacity - head + i] = data[i];
				}
			}
			else if (head < tail)
			{
				brojIf = 2;
				for (int i = head; i <= tail; i++)
				{
					if (*(char*)(data + i - head) == 0)
					{
						printf("lala\n");
					}
					newData[i - head] = data[i];
				}
			}
			else
			{
				brojIf = 3;
				int dataIndex = head;
				newData[0] = data[dataIndex];
				for (int i = 1; i < count; i++)
				{
					dataIndex = (dataIndex + 1) % capacity;
					newData[i] = data[dataIndex];
				}
			}
			capacity = newCapacity;
			head = 0;
			tail = count - 1;
			free(data);
			data = newData;
			/*for (int i = 0; i < count; i++)
			{
				if (*(char*)(data + i) == 0)
				{
					printf("Cought you! brojIf = %d", brojIf);
				}
			}*/
		}
	}


};

