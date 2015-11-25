#pragma once

/**
* @file ThreadSafeQueue.h
*
* @brief Reprezents thread safe circular buffer
*
*/

/**
* @class ThreadSafeQueue
* @brief Reprezents thread safe circular buffer
*
* This class enables usage of thread safe circular buffer by enqueue and dequeue
*
*/

#include "stdafx.h"

#define INITIAL_SIZE 4
#define QUEUE_IS_EMPTY -2147483648

template <class T>
class ThreadSafeQueue
{
public:

	/**
	* @brief Constructor of ThreadSafeQueue
	*/
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

	/**
	* @brief Constructor of ThreadSafeQueue
	*/
	ThreadSafeQueue(int initialSize)
	{
		head = 0;
		tail = -1;
		InitializeCriticalSection(&criticalSection);
		count = 0;
		data = static_cast< T* >( calloc(initialSize, sizeof(T)) );
		capacity = initialSize;
	}

	/**
	* @brief Constructor of ThreadSafeQueue
	*/
	ThreadSafeQueue()
	{
		head = 0;
		tail = -1;
		InitializeCriticalSection(&criticalSection);
		capacity = INITIAL_SIZE;
		data = static_cast< T* >( calloc(INITIAL_SIZE, sizeof(T)) );
		count = 0;
	}

	/**
	* @brief Destructor of ThreadSafeQueue
	*/
	~ThreadSafeQueue()
	{
		DeleteCriticalSection(&criticalSection);

		free(data);
	}


	/**
	* @brief Allows access to number of elements in circular buffer
	*
	*
	* @return Returns count
	*
	*/
	int GetCount()
	{
		EnterCriticalSection(&criticalSection);
		int retVal = count;
		LeaveCriticalSection(&criticalSection);
		return retVal;
	}

	/**
	* @brief Adds an element to circular buffer on tail
	*
	*@param *newData - represents new data that will be stored in circular buffer
	*
	*/
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

	/**
	* @brief Returns an element from circular buffer that is on the head
	*
	* @return Returns T (template class)
	*
	*/
	T* Dequeue()
	{
		EnterCriticalSection(&criticalSection);
		T *retVal = static_cast<T*>(malloc(sizeof(T)));

		if (count > 0)
		{
			*retVal = data[head];
			head = (head + 1) % capacity;
			count--;
			
			if (count <= capacity / 4)
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

	/**
	* @brief Resizes the circular buffer
	*
	*@param newCapacity - New capacity of the circular buffer
	*
	*/
	void Resize(int newCapacity)
	{
		if (newCapacity >= INITIAL_SIZE)
		{
			T *newData =  static_cast< T* >( calloc(newCapacity,sizeof(T)) );
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
			}
			else if (head < tail)
			{
				for (int i = head; i <= tail; i++)
				{
					newData[i - head] = data[i];
				}
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
			}
			capacity = newCapacity;
			head = 0;
			tail = count - 1;
			free(data);
			data = newData;
		}
	}


};

