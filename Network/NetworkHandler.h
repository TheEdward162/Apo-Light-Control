#pragma once

#include "../Unit/LightUnit.h"

class NetworkHandler {
public:
	typedef struct {
		uint32_t magic;
		uint32_t version;
		uint32_t msgType;

		uint32_t rgbCeiling;
		uint32_t rgbWall;
		char description[16];
		uint16_t image[256];
	} BroadcastMessage;

	typedef struct {
		uint32_t magic;
		uint32_t version;
		uint32_t msgType;

		int16_t valuesCeiling[3];
		int16_t valuesWall[3];
	} ControlMessage;

	NetworkHandler();
	~NetworkHandler();

	bool broadcastMessage(NetworkHandler::BroadcastMessage* message);
	bool sendMessage(NetworkHandler::ControlMessage* message, long ip);

	NetworkHandler::BroadcastMessage buildBroadcastMessage(LightUnit& unit);
	NetworkHandler::ControlMessage buildControlMessage(int type, int16_t valuesCeiling[], int16_t valuesWall[]);

private:
	const uint32_t MAGIC_NUMBER = 0xDEADBEEF;
	const uint32_t VERSION = 0x00010000;

	const int SOCKET_TIMEOUT_SECONDS = 0;
	const int SOCKET_TIMEOUT_uSECONDS = 100;
	const int SOCKET_PORT = 55555;

	int socketFD;
};