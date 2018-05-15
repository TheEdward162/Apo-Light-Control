#include "LedController.h"

#include "Mapper.h"
#include "MZRegisters.h"

const Mapper mapper = Mapper(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE);

void LedController::setLED1(uint32_t color) {
	*(volatile uint32_t*)(mapper.mem_base + SPILED_REG_LED_RGB1_o) = color;
}
void LedController::setLED2(uint32_t color) {
	*(volatile uint32_t*)(mapper.mem_base + SPILED_REG_LED_RGB2_o) = color;
}
void LedController::setLEDLine(uint32_t bits) {
	*(volatile uint32_t*)(mapper.mem_base + SPILED_REG_LED_LINE_o) = bits;
}