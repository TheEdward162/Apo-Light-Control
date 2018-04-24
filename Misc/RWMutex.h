#pragma once

#include <atomic>
#include <mutex>

class RWMutex {
public:
	RWMutex();
	~RWMutex();

	void lockRead();
	void unlockRead();

	void lockWrite();
	void unlockWrite();
private:
	std::atomic_int counter;
	std::atomic_bool isWaiting;
	std::mutex write;
};