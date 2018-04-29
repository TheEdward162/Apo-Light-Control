#pragma once

#include <list>
#include <unordered_map>
#include <string>

#include "Unit/LightUnit.h"
#include "Misc/ControlMessageQueue.h"

namespace Engine {
	extern std::list<LightUnit> unitList;
	extern ControlMessageQueue controlQueue;

	extern std::unordered_map<std::string, std::array<uint16_t, 256>> uiIcons;

	int run(int argc, char** argv);
}