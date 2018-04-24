#pragma once

#include <cstdint>

#include <queue>
#include <atomic>
#include <mutex>

class ControlMessageQueue {
public:
	typedef struct ControlMessageInfo {
		uint32_t ip;
		int type;
		int16_t valuesCeiling[3];
		int16_t valuesWall[3];

		ControlMessageInfo() {
			ip = 0;
		}
	} ControlMessageInfo;

	ControlMessageQueue();
	~ControlMessageQueue();

	bool hasChanged();
	size_t size();

	void enqueue(ControlMessageInfo info);
	ControlMessageInfo dequeue();
private:
	std::queue<ControlMessageInfo> q;
	std::atomic_bool changed;
	std::mutex mutex_change;
};