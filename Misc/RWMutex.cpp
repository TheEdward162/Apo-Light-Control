#include "RWMutex.h"

#include <thread>

RWMutex::RWMutex() {
	counter = 0;
	isWaiting = false;
}
RWMutex::~RWMutex() {

}

void RWMutex::lockRead() {
	while (isWaiting) {
		std::this_thread::yield();
	}

	if (counter == 0) {
		write.lock();
	}
	counter++;
}
void RWMutex::unlockRead() {
	if (counter == 1) {
		write.unlock();
		counter--;
	}
}

void RWMutex::lockWrite() {
	isWaiting = true;
	write.lock();
	isWaiting = false;
}
void RWMutex::unlockWrite() {
	write.unlock();
}