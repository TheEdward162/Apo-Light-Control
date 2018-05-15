#pragma once

#include <atomic>
#include <mutex>

/**
	@addtogroup network_thread
	@{
*/

/**
	@brief Read/Write mutex class.
*/
class RWMutex {
public:
	RWMutex();
	~RWMutex();

	/**
		@brief Locks this mutex for reading. This is a shared lock.
		@details This is a shared mutex and it's implemented as a counter that is incremented on lock and decremented on unlock.
		This class also implements an anti-writer-starvation mechanism (write-preferring RW lock).
	*/
	void lockRead();
	/**
		@brief Unlocks one reader lock.
		@details Decrements internal reader counter.
	*/
	void unlockRead();

	/**
		@brief Locks write mutex.
		@details This is an exclusive mutex. It is a write-preferring lock.
	*/
	void lockWrite();
	/**
		@brief Unlocks write mutex.
	*/
	void unlockWrite();
private:
	std::atomic_int counter;
	std::atomic_bool isWaiting;
	std::mutex write;
};
/** @} */