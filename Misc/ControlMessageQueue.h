#pragma once

#include <cstdint>
#include <cstring>

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
			this->ip = 0;
			this->type = -1;
			memset(valuesCeiling, 0, sizeof(int16_t) * 3);
			memset(valuesWall, 0, sizeof(int16_t) * 3);
		}

		ControlMessageInfo(uint32_t ip) : ControlMessageInfo() {
			this->ip = ip;
		}
		
		ControlMessageInfo(uint32_t ip, int type) : ControlMessageInfo(ip) {
			this->type = type;
			if (type == 2) {
				valuesCeiling[0] = -1;
				valuesCeiling[1] = -1;
				valuesCeiling[2] = -1;

				valuesWall[0] = -1;
				valuesWall[1] = -1;
				valuesWall[2] = -1;
			}
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