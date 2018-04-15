#include <cstring>

#include "LightUnit.h"

LightUnit::LightUnit() {
	memset(&this->description, '\0', 17);
	memset(&this->image, 0, 256 * sizeof(uint16_t));

	lastNetworkBroadcastTimePoint = std::chrono::steady_clock::now();
}

LightUnit::LightUnit(LightUnit&& other) {
	// lock the other mutex
	std::lock_guard<std::mutex> otherLock(other.mutex_change);

	ip = std::move(other.ip);
	memcpy(&description, &other.description, 17);
	memcpy(&image, &other.image, 256 * sizeof(uint16_t));
	rgbCeiling = std::move(other.rgbCeiling);
	rgbWall = std::move(other.rgbWall);
	lastNetworkBroadcastTimePoint = std::move(other.lastNetworkBroadcastTimePoint);
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

LightUnit& LightUnit::operator=(LightUnit&& other) {
	if (this != &other) {
		std::unique_lock<std::mutex> selfLock(mutex_change, std::defer_lock);
		std::unique_lock<std::mutex> otherLock(other.mutex_change, std::defer_lock);
		// to avoid deadlocking, defer locks and then call std::lock
		std::lock(selfLock, otherLock);

		ip = std::move(other.ip);
		memcpy(&description, &other.description, 17);
		memcpy(&image, &other.image, 256 * sizeof(uint16_t));
		rgbCeiling = std::move(other.rgbCeiling);
		rgbWall = std::move(other.rgbWall);
		lastNetworkBroadcastTimePoint = std::move(other.lastNetworkBroadcastTimePoint);
	}
	
	return *this;
}
