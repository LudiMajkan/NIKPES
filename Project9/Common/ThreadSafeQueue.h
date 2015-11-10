#pragma once

#define INITIAL_SIZE 100

template <class T>
class ThreadSafeQueue
{
protected:
	T data[];
	int count = 0;
	int capacity = INITIAL_SIZE;
	int head = 0;
	int tail = 0;

	CRITICAL_SECTION criticalSection;
	
	void Resize(int newCapacity);

public:
	ThreadSafeQueue(T* dataToCopy, int size);
	ThreadSafeQueue(int initialSize);
	ThreadSafeQueue();

	~ThreadSafeQueue();

	bool Enqueue(T newData);
	T Dequeue();
	void Clear();
};

