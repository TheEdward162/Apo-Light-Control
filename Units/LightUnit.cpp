#include "LightUnit.h"
#include <cstring>

LightUnit::LightUnit(char description[16], uint16_t image[256]) {
	memcpy(&this->description, description, 16);
	memcpy(&this->image, image, 256);
}

LightUnit::~LightUnit() {

}
