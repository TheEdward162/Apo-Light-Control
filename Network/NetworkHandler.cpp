#include <cstdint>
#include <cstring>

// for close
#include <unistd.h>
#include <sys/socket.h>
// for inet_addr, hton, ntoh
#include <arpa/inet.h>
#include <ifaddrs.h>

#include <stdexcept>
#include <algorithm>
#include <iostream>

#include "NetworkHandler.h"

NetworkHandler::NetworkHandler() {
	// open socket
	socketFD = socket(AF_INET, SOCK_DGRAM, 0);
	if (socketFD == -1) {
		throw std::runtime_error("Could not open socket");
	}

	// bind socket so we can recieve broadcasts
	struct sockaddr_in bindaddr;

	memset(&bindaddr, 0, sizeof(bindaddr));
	bindaddr.sin_family = AF_INET;
	bindaddr.sin_port = htons(SOCKET_PORT);
	bindaddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(socketFD, (struct sockaddr *) &bindaddr, sizeof(bindaddr)) == -1) {
		throw std::runtime_error("Could not bind socket");
	}

	// Set socket broadcast permission
	int broadcastEnable = 1;
	if (setsockopt(socketFD, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable))) {
		throw std::runtime_error("Could not set socket broadcast permission");
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
	
	// get local IPs to filter when recieving broadcasts
	localIPs = std::vector<unsigned long>(2);

	struct ifaddrs *myaddrs, *ifa;
    if (getifaddrs(&myaddrs) != 0) {
        throw std::runtime_error("Could not get local IP");
    }

    for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

		if (ifa->ifa_addr->sa_family == AF_INET) {
			struct sockaddr_in *s4 = (struct sockaddr_in *)ifa->ifa_addr;
			in_addr_t ipAddress = s4->sin_addr.s_addr;

			std::cout << "Local IP: ";
			printIP(ipAddress);

			localIPs.push_back(s4->sin_addr.s_addr);
		}
    }

    freeifaddrs(myaddrs);
}

NetworkHandler::~NetworkHandler() {
	close(socketFD);
}

bool NetworkHandler::broadcastMessage(const NetworkHandler::BroadcastMessage* message) {
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

bool NetworkHandler::broadcastUnit(LightUnit& unit) {
	NetworkHandler::BroadcastMessage message = buildBroadcastMessage(unit);
	return broadcastMessage(&message);
}

bool NetworkHandler::sendMessage(const NetworkHandler::ControlMessage* message, unsigned long ip) {
	struct sockaddr_in address;

    memset(&address, '\0', sizeof(struct sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_port = htons(SOCKET_PORT);
    address.sin_addr.s_addr = ip;

	int result = sendto(socketFD, message, sizeof(NetworkHandler::ControlMessage), 0, (struct sockaddr*) &address, sizeof(struct sockaddr_in));
	if (result < 0)
		return false;

	return true;
}

NetworkHandler::RecievedMessage NetworkHandler::recieveMessage() {
	NetworkHandler::RecievedMessage recievedMessage;
	
	// buffer big enough to fit all message types
	const int messageBufferLength = sizeof(NetworkHandler::BroadcastMessage);
	char messageBuffer[messageBufferLength];

	struct sockaddr_storage sourceAddress;
	socklen_t sourceAddressSize;

	int recievedBytes = recvfrom(socketFD, messageBuffer, messageBufferLength, 0, (sockaddr*) &sourceAddress, &sourceAddressSize);
	if (recievedBytes < (int)sizeof(NetworkHandler::Message)) {
		return recievedMessage;
	}

	// check for MAGIC_NUMBER and VERSION
	NetworkHandler::Message message;
	memcpy(&message, messageBuffer, sizeof(NetworkHandler::Message));
	message.msgType = ntohl(message.msgType);
	if (message.magic != MAGIC_NUMBER || message.version != VERSION) {
		return recievedMessage;
	}
	recievedMessage.ip = getIPFromStorage(&sourceAddress);

	// filter out messages from self
	if (std::find(localIPs.begin(), localIPs.end(), recievedMessage.ip) != localIPs.end()) {
		recievedMessage.ip = 0;
		return recievedMessage;
	}

	if (message.msgType == 0) {
		recievedMessage.pMessage = new NetworkHandler::BroadcastMessage();
		memcpy(recievedMessage.pMessage, messageBuffer, sizeof(NetworkHandler::BroadcastMessage));
		recievedMessage.pMessage->msgType = ntohl(recievedMessage.pMessage->msgType);
	} else if (message.msgType == 1 || message.msgType == 2) {
		recievedMessage.pMessage = new NetworkHandler::ControlMessage();
		memcpy(recievedMessage.pMessage, messageBuffer, sizeof(NetworkHandler::ControlMessage));
		recievedMessage.pMessage->msgType = ntohl(recievedMessage.pMessage->msgType);
	}

	// steady now
	recievedMessage.timePoint = std::chrono::steady_clock::now();
	return recievedMessage;
}

NetworkHandler::BroadcastMessage NetworkHandler::buildBroadcastMessage(LightUnit& unit) {
	BroadcastMessage message;
	message.magic = MAGIC_NUMBER;
	message.version = VERSION;
	message.msgType = htonl(0);

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
	message.msgType = htonl(type);

	memcpy(&message.valuesCeiling, valuesCeiling, 3);
	memcpy(&message.valuesWall, valuesWall, 3);

	return message;
}

unsigned long NetworkHandler::getIPFromStorage(const struct sockaddr_storage* address) {
	struct sockaddr_in* sin = (struct sockaddr_in*) address;
	return sin->sin_addr.s_addr;
}

void NetworkHandler::printIP(unsigned long ip) {
	unsigned char* pChar = (unsigned char*)&ip;
	std::cout << (unsigned int)(pChar[0]) << " " << (unsigned int)(pChar[1]) << " " << (unsigned int)(pChar[2]) << " " << (unsigned int)(pChar[3]) << std::endl;
}