#pragma once

#include <cstdint>

// for inet_addr, hton, ntoh
#include <arpa/inet.h>

#include <chrono>
#include <vector>

#include "../Unit/LightUnit.h"

class NetworkHandler {
public:
	typedef struct Message {
		uint32_t magic;
		uint32_t version;
		uint32_t msgType;
	} Message;

	typedef struct BroadcastMessage : Message {
		uint32_t rgbCeiling;
		uint32_t rgbWall;
		char description[16];
		uint16_t image[256];
	} BroadcastMessage;

	typedef struct ControlMessage : Message {
		int16_t valuesCeiling[3];
		int16_t valuesWall[3];
	} ControlMessage;

	typedef struct RecievedMessage {
		uint32_t ip;
		std::chrono::steady_clock::time_point timePoint;
		Message* pMessage;

		RecievedMessage() {
			ip = 0;
			timePoint = std::chrono::steady_clock::time_point::min();
			pMessage = NULL;
		}

		~RecievedMessage() {
			// My pointer dies with me!
			if (pMessage != NULL) {
				delete pMessage;
			}
			// Seriously, though, pay attention to this, pMessage is deleted when RecievedMessage goes out of scope
		}
	} RecievedMessage;

	NetworkHandler();
	~NetworkHandler();

	bool broadcastMessage(const BroadcastMessage* message);
	bool broadcastUnit(LightUnit& unit);

	bool sendMessage(const ControlMessage* message, uint32_t ip);

	BroadcastMessage buildBroadcastMessage(LightUnit& unit);
	ControlMessage buildControlMessage(int type, int16_t valuesCeiling[], int16_t valuesWall[]);

	RecievedMessage recieveMessage();

private:
	const uint32_t MAGIC_NUMBER = htonl(0xDEADBEEF);
	const uint32_t VERSION = htonl(0x00010000);

	const int SOCKET_TIMEOUT_SECONDS = 0;
	const int SOCKET_TIMEOUT_uSECONDS = 100;
	const int SOCKET_PORT = 55555;

	int socketFD;
	std::vector<uint32_t> localIPs;

	uint32_t getIPFromStorage(const struct sockaddr_storage* address);
	void printIP(uint32_t ip);
};