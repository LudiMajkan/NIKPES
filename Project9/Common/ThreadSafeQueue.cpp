#include "stdafx.h"
#include "ThreadSafeQueue.h"

template <class T>
ThreadSafeQueue<T>::ThreadSafeQueue()
{
	InitializeCriticalSection(&criticalSection);
	
	data = new T[INITIAL_SIZE];
}

template <class T>
ThreadSafeQueue<T>::ThreadSafeQueue(int initialSize)
{
	InitializeCriticalSection(&criticalSection);

	data = new T[initialSize];
	capacity = initialSize;
}

template <class T>
ThreadSafeQueue<T>::ThreadSafeQueue(T* dataToCopy, int newCapacity)
{
	InitializeCriticalSection(&criticalSection);

	if (newCapacity >= capacity)
	{
		capacity = newCapacity;
	}

	data = new T[INITIAL_SIZE];
	for (int i = 0; i < size; i++)
	{
		Enqueue(dataToCopy[i]);
	}
}

template <class T>
ThreadSafeQueue<T>::~ThreadSafeQueue()
{
	DeleteCriticalSection(&criticalSection);

	delete(data);
}

template <class T>
bool Enqueue(T newData)
{
	tail = (tail + 1) % capacity;
	EnterCriticalSection(&criticalSection);
	if (tail == head)
	{
		Resize(capacity * 2);
	}
	else
	{
		data[tail] = newData;
	}

	LeaveCriticalSection(&criticalSection);
}

template <class T>
T Dequeue()
{
	EnterCriticalSection(&criticalSection);
	
	T retVal = data[head];
	head = (head + 1) % capacity;
	
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
		cout << "Queue is empty";
	}
	
	LeaveCriticalSection(&criticalSection);
	
	return retVal;
}

template <class T>
void Resize(int newCapacity)
{
	EnterCriticalSection(&criticalSection);

	T newData[newCapacity];
	for (int i = head; i < capacity; i++)
	{
		newData[i - head] = data[i];
	}
	for (int i = 0; i <= tail; i++)
	{
		newData[capacity - head + i] = data[i];
	}
	tail = capacity;
	capacity = newCapacity;
	head = 0;

	delete(data);
	data = newData;

	LeaveCriticalSection(&criticalSection);
}
