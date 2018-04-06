#include <cstdint>
#include <cstring>

// for close
#include <unistd.h>
#include <sys/socket.h>
// for inet_addr
#include <arpa/inet.h>

#include <stdexcept>

#include "NetworkHandler.h"

NetworkHandler::NetworkHandler() {
	socketFD = socket(AF_INET, SOCK_DGRAM, 0);
	if (socketFD == -1) {
		throw std::runtime_error("Could not open socket");
	}

	// Set socket timeout
	if (SOCKET_TIMEOUT_SECONDS + SOCKET_TIMEOUT_uSECONDS > 0) {
		struct timeval tv;
		tv.tv_sec = SOCKET_TIMEOUT_SECONDS;
		tv.tv_usec = SOCKET_TIMEOUT_uSECONDS;
		if (setsockopt(socketFD, SOL_SOCKET, SO_RCVTIMEO, (char*) &tv, sizeof tv)) {
			throw std::runtime_error("Could not set socket timeout");
		}
	}

	// Set socket broadcast permission
	int broadcastEnable = 1;
	if (setsockopt(socketFD, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable))) {
		throw std::runtime_error("Could not set socket broadcast permission");
	}
}

NetworkHandler::~NetworkHandler() {
	close(socketFD);
}

bool NetworkHandler::broadcastMessage(NetworkHandler::BroadcastMessage* message) {
	struct sockaddr_in address;

    memset(&address, '\0', sizeof(struct sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_port = htons(SOCKET_PORT);
    address.sin_addr.s_addr = htonl(INADDR_BROADCAST);

	int result = sendto(socketFD, message, sizeof(NetworkHandler::BroadcastMessage), 0, (struct sockaddr*) &address, sizeof(struct sockaddr_in));
	if (result < 0)
		return false;

	return true;
}

bool NetworkHandler::sendMessage(NetworkHandler::ControlMessage* message, long ip) {
	struct sockaddr_in address;

    memset(&address, '\0', sizeof(struct sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_port = htons(SOCKET_PORT);
    address.sin_addr.s_addr = ip;

	int result = sendto(socketFD, message, sizeof(NetworkHandler::ControlMessage), 0, (struct sockaddr*) &address, sizeof(struct sockaddr_in));
	if (result < 0)
		return false;

	return true;
	
	return true;
}

NetworkHandler::BroadcastMessage NetworkHandler::buildBroadcastMessage(LightUnit& unit) {
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

NetworkHandler::ControlMessage NetworkHandler::buildControlMessage(int type, int16_t valuesCeiling[3], int16_t valuesWall[3]) {
	ControlMessage message;
	message.magic = MAGIC_NUMBER;
	message.version = VERSION;
	message.msgType = 0;

	memcpy(&message.valuesCeiling, valuesCeiling, 3);
	memcpy(&message.valuesWall, valuesWall, 3);

	return message;
}