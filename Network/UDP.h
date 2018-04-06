#include "../Units/LightUnit.h"

class SocketHandler {
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

	SocketHandler();
	~SocketHandler();

	SocketHandler::BroadcastMessage buildBroadcastMessage(LightUnit& unit);
	SocketHandler::ControlMessage buildControlMessage(int type, int16_t valuesCeiling[], int16_t valuesWall[]);

private:
	int socketFD;
}