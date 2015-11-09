#include "stdafx.h"
#include "ThreadSafeQueue.h"

template <class T>
ThreadSafeQueue<T>::ThreadSafeQueue()
{
	data = new T[INITIAL_SIZE];
}

template <class T>
ThreadSafeQueue<T>::ThreadSafeQueue(int initialSize)
{
	data = new T[initialSize];
	capacity = initialSize;
}

template <class T>
ThreadSafeQueue<T>::ThreadSafeQueue(T* dataToCopy, int newCapacity)
{
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
	delete(data);
}

template <class T>
bool Enqueue(T newData)
{
	tail = (tail + 1) % capacity;
	if (tail == head)
	{
		Resize(capacity * 2);
	}
	else
	{
		data[tail] = newData;
	}
}

template <class T>
T Dequeue()
{
	T retVal = data[head];
	head = (head + 1) % capacity;
	
	if (head < tail)
	{

	}
	else if (head > tail)
	{

	}
	else
	{
		// Ovo je samo za probavanje koda...
		cout << "Queue is empty";
	}
	
	/*if (/)
	{
		Resize(capacity / 2);
	}
	else
	{
		return retval;
	}*/


	return retVal;
}

template <class T>
void Resize(int newCapacity)
{
	T newData[newCapacity];
	for (int i = head; i < capacity; i++)
	{
		newData[i - head] = data[i];
	}
	for (int i = 0; i < head; i++)
	{
		newData[capacity - head + i] = data[i];
	}
	tail = capacity;
	capacity = newCapacity;
	head = 0;

	delete(data);
	data = newData;
}
