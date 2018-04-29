#pragma once

#include <cstdint>
#include <cstring>

#include <queue>
#include <atomic>
#include <mutex>

/**
	@addtogroup network_thread
	@{
*/

/**
	@brief Thread-safe FIFO queue for control messages.
*/
class ControlMessageQueue {
public:
	/**
		@brief Element of the ControlMessageQueue.
		@see NetworkHandler::Message
		@see NetworkHandler::ControlMessage
	*/
	typedef struct ControlMessageInfo {
		/**
			@brief IP address
		*/
		uint32_t ip;
		/**
			@brief Message type
		*/
		int type;
		/**
			@brief Values ceiling
		*/
		int16_t valuesCeiling[3];
		/**
			@brief Values wall
		*/
		int16_t valuesWall[3];

		/**
			@brief Empty constructor constructs invalid ControlMessageInfo
		*/
		ControlMessageInfo() {
			this->ip = 0;
			this->type = -1;
			memset(valuesCeiling, 0, sizeof(int16_t) * 3);
			memset(valuesWall, 0, sizeof(int16_t) * 3);
		}

		/**
			@brief ControlMessageInfo constructor with ip and optional type
			@details If type is set to 2, this constructor also sets all values to -1.
			@param[in] ip
			@param[in] type
		*/		
		ControlMessageInfo(uint32_t ip, int type=-1) : ControlMessageInfo() {
			this->ip = ip;
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

	/**
		@brief Checks if the queue has changed.
		@returns Value indicating wether the queue has changed since the last call to this function.
	*/
	bool hasChanged();

	/**
		@brief Returns number of elements in the queue
		@returns Number of elements in the queue.
	*/
	size_t size();

	/**
		@brief Equeues an element at the back.
		@param[in] info
	*/
	void enqueue(ControlMessageInfo info);

	/**
		@brief Dequeues an element from the front.
		@returns The front element. If there are no elements, returns ControlMessageInfo();
	*/
	ControlMessageInfo dequeue();
private:
	std::queue<ControlMessageInfo> q;
	std::atomic_bool changed;
	std::mutex mutex_change;
};
/** @} */