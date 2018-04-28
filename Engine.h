#pragma once

#include <vector>

#include "Unit/LightUnit.h"
#include "Misc/ControlMessageQueue.h"

/** Class representing the entry point of the whole application.
 * It stores a list of connected units and a control message queue.
 * */
namespace Engine {

	extern std::vector<LightUnit> unitList; /**< The list of all currently connected units. */
	extern ControlMessageQueue controlQueue; /**< The control message queue. */

	/**
	 * @brief The entry point of the whole application. Connects a new unit to the network.
	 * @argc The number of arguments.
	 * @argv The command line arguments. The format should be  '"Unit Description" path_to_icon16x16.ppm'.
	 * */
	int run(int argc, char** argv);
}