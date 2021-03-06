//
// Created by klara on 7.4.18.
//

#pragma once

#include "Mapper.h"
#include "MZRegisters.h"

/**
	@addtogroup unit
	@{
*/

/**
	@brief Handler device input.
**/
class DeviceInput {
public:
    /**
     * Constructor.
     * */
    DeviceInput();
    /**
     * Destructor.
     * */
    ~DeviceInput();

	/**
		@brief The change in the device knob positions.
	*/
    int8_t rgbDelta[3] = {0};
	/**
		@brief Whether given device knob is pressed or not.
	*/
    bool knobsPressed[3] = {0};

    /**
     * @brief Gets the input (knobs state) from the device.
     * */
    void update();
    
    /**
     * @brief Locks currently pressed knobs to 0 and unlocks them on release.
     * */
    void lock();

private:
    bool locked[3] = {0};

	uint8_t lastKnobValues[3] = {0};
    Mapper mapper = Mapper(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE);

    void processDelta(uint8_t value, size_t index);
    void checkLock(bool actual, size_t index);
};
/** @} */