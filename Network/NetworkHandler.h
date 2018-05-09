#pragma once

#include <cstdint>

// for inet_addr, hton, ntoh
#include <arpa/inet.h>

#include <chrono>
#include <vector>

#include "../Unit/LightUnit.h"

/**
	@defgroup network_thread Networking and threading module
	@brief Networking and threading.
*/

/**
	@addtogroup network_thread
	@{
*/

/**
	@brief Class that handles all network communication.
*/
class NetworkHandler {
public:
	/**
		@brief Base message struct.
	*/
	typedef struct Message {
		/**
			@brief Magic number to detect packets.
		*/
		uint32_t magic;
		/**
			@brief Version.
		*/
		uint32_t version;
		/**
			@brief Type of message.
		*/
		uint32_t msgType;
	} Message;

	/**
		@brief Broadcast message. Type 0.
	*/
	typedef struct BroadcastMessage : Message {
		/**
			@brief RGB ceiling value of sender unit.
		*/
		uint32_t rgbCeiling;
		/**
			@brief RGB wall value of sender unit.
		*/
		uint32_t rgbWall;
		/**
			@brief Description of sender unit.
		*/
		char description[16];
		/**
			@brief Icon of sender unitt
		*/
		uint16_t image[256];
	} BroadcastMessage;

	/**
		@brief Control message. Types 1 and 2.
		@details
		Type 1 increments values.
		Type 2 sets values.
	*/
	typedef struct ControlMessage : Message {
		/**
			@brief Ceiling values to increment/set for the recieving unit.
		*/
		int16_t valuesCeiling[3];
		/**
			@brief Wall values to increment/set for the recieving unit.
		*/
		int16_t valuesWall[3];
	} ControlMessage;

	/**
		@brief Struct that represents a recieved message.
	*/
	typedef struct RecievedMessage {
		/**
			@brief IP address of the sender. Set to 0 to represent invalid message (e.g. on timeout).
		*/
		uint32_t ip;
		/**
			@brief Time point at which the message was recieved.
		*/
		std::chrono::steady_clock::time_point timePoint;
		/**
			@brief Pointer to the message.
		*/
		Message* pMessage;

		RecievedMessage() {
			ip = 0;
			timePoint = std::chrono::steady_clock::time_point::min();
			pMessage = NULL;
		}

		/**
			@brief Desctructor. Deletes pMessage.
		*/
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

	/**
		@brief Broadcasts message.
		@param[in] message
		@returns Value indicating if the message was sent successfully.
	*/
	bool broadcastMessage(const BroadcastMessage* message);
	/**
		@brief Builds and broadcasts message for given unit.
		@param[in] unit
		@returns Value indicating if the message was sent successfully.
	*/
	bool broadcastUnit(LightUnit& unit);

	/**
		@brief Sends control message.
		@param[in] message
		@param[in] ip
		@returns Value indicating if the message was sent successfully.
	*/
	bool sendMessage(const ControlMessage* message, uint32_t ip);

	/**
		@brief Builds broadcast message for unit.
		@param[in] unit
		@returns Built broadcast message.
	*/
	BroadcastMessage buildBroadcastMessage(LightUnit& unit);

	/**
		@brief Builds control message.
		@param[in] type Type of the message
		@param[in] valuesCeiling Ceiling values to set in the message.
		@param[in] valuesWall Wall values to set in the message.
		@see Message
		@returns Built control message.
	*/
	ControlMessage buildControlMessage(int type, int16_t valuesCeiling[], int16_t valuesWall[]);

	/**
		@brief Waits for a message on the socket. Can timeout.
		@returns RecievedMessage only valid when ip != 0.
	*/
	RecievedMessage recieveMessage();

private:
	const uint32_t MAGIC_NUMBER = htonl(0x414c4331);
	const uint32_t VERSION = htonl(0x00010000);

	const int SOCKET_TIMEOUT_SECONDS = 0;
	const int SOCKET_TIMEOUT_uSECONDS = 100;
	const int SOCKET_PORT = 55555;

	int socketFD;
	std::vector<uint32_t> localIPs;

	uint32_t getIPFromStorage(const struct sockaddr_storage* address);
	void printIP(uint32_t ip);
};
/** @} */