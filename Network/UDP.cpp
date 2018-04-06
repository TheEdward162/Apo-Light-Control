#include <cstdint>
#include <cstring>

// for close
#include <unistd.h>
#include <sys/socket.h>
// for inet_addr
#include <arpa/inet.h>

#include "UDP.h"

SocketHandler::SocketHandler() {
	socketFD = socket(AF_INET, SOCK_DGRAM, 0);
	if (socketFD == -1) {
		// error
	}

	if (SOCKET_TIMEOUT > 0) {
		struct timeval tv;
		tv.tv_sec = SOCKET_TIMEOUT;
		tv.tv_usec = 0;
		if (setsockopt(socketFD, SOL_SOCKET, SO_RCVTIMEO, (char*) &tv, sizeof tv)) {
			// Error
		}
	}
}

SocketHandler::~SocketHandler() {
	close(socketFD);
}

SocketHandler::BroadcastMessage SocketHandler::buildBroadcastMessage(LightUnit& unit) {
	BroadcastMessage message;
	message.magic = MAGIC_NUMBER;
	message.version = VERSION;
	message.msgType = 0;

	message.rgbCeiling = unit.rgbCeiling;
	message.rgbWall = unit.rgbWall;

	memcpy(&message.description, &unit.description, 16);
	memcpy(&message.image, &unit.image, 256);

	return message;
}

SocketHandler::ControlMessage SocketHandler::buildControlMessage(int type, int16_t valuesCeiling[3], int16_t valuesWall[3]) {
	ControlMessage message;
	message.magic = MAGIC_NUMBER;
	message.version = VERSION;
	message.msgType = 0;

	memcpy(&message.valuesCeiling, valuesCeiling, 3);
	memcpy(&message.valuesWall, valuesWall, 3);

	return message;
}