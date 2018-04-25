#pragma once

#include <vector>

#include "Unit/LightUnit.h"
#include "Misc/ControlMessageQueue.h"

namespace Engine {
	extern std::vector<LightUnit> unitList;
	extern ControlMessageQueue controlQueue;

	int run(int argc, char** argv);
}