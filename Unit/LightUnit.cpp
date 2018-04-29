#include <cstring>

#include "LightUnit.h"

LightUnit::LightUnit() {
	memset(&this->description, '\0', 17);
	memset(&this->image, 0, 256 * sizeof(uint16_t));

	lastNetworkBroadcastTimePoint = std::chrono::steady_clock::now();
	screenActive = false;
}
LightUnit::LightUnit(const char description[16]) : LightUnit() {
	int descriptionLength = strlen(description);
	if (descriptionLength > 16)
		descriptionLength = 16;
	memcpy(&this->description, description, descriptionLength);
}
LightUnit::LightUnit(unsigned long ip, const char description[16], const uint16_t image[256]) : LightUnit(description) {
	this->ip = ip;

	memcpy(&this->image, image, 256 * sizeof(uint16_t));
}
LightUnit::LightUnit(unsigned long ip, const char description[16], const uint16_t image[256], uint32_t rgbCeiling, uint32_t rgbWall) : LightUnit(ip, description, image) {
	this->rgbCeiling = rgbCeiling;
	this->rgbWall = rgbWall;
}

LightUnit::~LightUnit() {

}
