#include "ControlMessageQueue.h"

ControlMessageQueue::ControlMessageQueue() {
	changed = false;
}

ControlMessageQueue::~ControlMessageQueue() {

}

bool ControlMessageQueue::hasChanged() {
	bool val = changed;
	changed = false;

	return val;
}

size_t ControlMessageQueue::size() {
	std::lock_guard<std::mutex> lock(mutex_change);
	return q.size();
}

void ControlMessageQueue::enqueue(ControlMessageQueue::ControlMessageInfo info) {
	std::lock_guard<std::mutex> lock(mutex_change);

	q.push(std::move(info));
	changed = true;
}

ControlMessageQueue::ControlMessageInfo ControlMessageQueue::dequeue() {
	std::lock_guard<std::mutex> lock(mutex_change);

	if (q.empty()) {
		return ControlMessageInfo();
	} else {
		ControlMessageInfo res = q.front();
		q.pop();

		return res;
	}
}