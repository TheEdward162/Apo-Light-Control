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
    int8_t RGBDelta[3];
	/**
		@brief Wether given device knob is pressed or not.
	*/
    bool RGBPressed[3]; 

    /**
     * @brief Gets the input (knobs state) from the device.
     * */
    void update();

private:
	uint32_t knobs_value;
    uint32_t prev_knobs_value;
    Mapper mapper = Mapper(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE);

    int8_t getDelta(uint8_t prev, uint8_t act);
};
/** @} */