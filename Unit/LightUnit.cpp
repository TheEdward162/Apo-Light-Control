#include "LightUnit.h"
#include <cstring>

LightUnit::LightUnit() {
	memset(&this->description, '\0', 17);
	memset(&this->image, 0, 256 * sizeof(uint16_t));
}

LightUnit::LightUnit(const char* description) : LightUnit() {
	int descriptionLength = strlen(description);
	if (descriptionLength > 16)
		descriptionLength = 16;
	memcpy(&this->description, description, descriptionLength);
}

LightUnit::~LightUnit() {

}
