#pragma once

#include <list>
#include <unordered_map>
#include <string>

#include "Unit/LightUnit.h"
#include "Misc/ControlMessageQueue.h"

/** 
	@brief Namesapce representing the entry point of the application.
	@details It stores a list of connected units, a control message queue and ui icons map.
	Also internally runs and synchronizes the main loop and network loop and handles network messages.
**/
namespace Engine {
	extern std::list<LightUnit> unitList; /*< The list of all currently connected units. */
	extern ControlMessageQueue controlQueue; /*< The control message queue. */

	extern std::unordered_map<std::string, std::array<uint16_t, 256>> uiIcons; /*< The map of UI icons. */

	/**
	 * @brief The entry point of the whole application.
	   @details This function starts a network thread, connects the new unit to the network and loads ui icons.
	 * @param[in] argc The number of arguments.
	 * @param[in] argv The command line arguments. The format should be  '"Unit Description" path_to_icon16x16.ppm'.
	 * */
	int run(int argc, char** argv);
}
/* namespace Engine */

/** @mainpage Apo Light Control
	@section intro_sec Introduction
	This is the documentation for the project Apo Light Control by Klára and Edward.
	You can find the source code on <a href="https://gitlab.fel.cvut.cz/lavusedu/APO_SEM">FEL Gitlab</a> (if you have the access rights, ofc ^-^).
 */