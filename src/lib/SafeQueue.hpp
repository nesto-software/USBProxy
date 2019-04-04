#ifndef SAFE_QUEUE
#define SAFE_QUEUE

// Based on http://stackoverflow.com/questions/15278343/c11-thread-safe-queue

#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <thread>

#define QUEUE_MAX 10000
#define QUEUE_FULL_WARNING_THRESHOLD 1000
#include <sys/time.h>

template <class T>
class SafeQueue
{
public:
	SafeQueue(void)
		: q()
		, qPriority()
		, m()
		, c()
		, destruct(0)
	{}

	~SafeQueue(void)
	{
	    destruct = true;
	    c.notify_one();
	    std::this_thread::yield();
	}

	void enqueue(T t) {
		std::lock_guard<std::mutex> lock(m);
		if (!q.empty()) {
			auto size(q.size());
			if (size >= QUEUE_MAX)
				return;
			if (size == QUEUE_MAX - QUEUE_FULL_WARNING_THRESHOLD)
				std::cerr << "Warning: queue fills up! Feel free to search the bug in either the driver or usbproxy.\n";
		}
		q.push(t);
		c.notify_one();
	}

	T dequeue(void) {
		std::unique_lock<std::mutex> lock(m);
		while(q.empty() && qPriority.empty()) {
			c.wait(lock);
			if(destruct)
			{
			    return nullptr;
			}
		}
		T val;
		if (!qPriority.empty()) {
			std::swap(qPriority.front(), val);
			qPriority.pop();
		} else {
			std::swap(q.front(), val);
			q.pop();
		}

		return val;
	}

	void enqueuePriority(T t) {
		std::lock_guard<std::mutex> lock(m);
		if (!qPriority.empty()) {
			auto size(qPriority.size());
			if (size >= QUEUE_MAX)
				return;
			if (size == QUEUE_MAX - QUEUE_FULL_WARNING_THRESHOLD)
				std::cerr << "Warning: queuePriority fills up! Feel free to search the bug in either the driver or usbproxy.\n";
		}
		qPriority.push(t);
		c.notify_one();
	}

	bool empty(void) {return q.empty() && qPriority.empty();}

	size_t  size(void) {return q.size() + qPriority.size();}

private:
	std::queue<T> q, qPriority;
	mutable std::mutex m;
	std::condition_variable c;
	bool destruct;
};
#endif
