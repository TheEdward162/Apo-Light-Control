//
// Created by klara on 21.4.18.
//

#pragma once

#include <cstdint>

/**
 * @brief Anything related to handling colours.
 *
 */
namespace Colour {
/*
    An enum containing basic RGB565 colours.
*/
    enum {
        BLACK = 0,
        WHITE = 0xFFFF,
        RED = 0xF800,
        ORANGE = 0xFC00,
        YELLOW = 0xFF80,
        LIME = 0xB7E0,
        GREEN = 0x4FE0,
        DARK_GREEN = 0x4D24,
        BLUE = 0x051F,
        PURPLE = 0x881F,
        BROWN = 0x9260,
        DARK_BLUE = 0x08CB,
        TURUOISE = 0x6694,
        UGLY = 0xEC1A,
        WEIRD_RED = 0xA165,
        DARK_GREY = 0x2945,
        LIGHT_GREY = 0xD69A,
        ALMOST_GOLD = 0xFE03
    };

    /** @brief Extracts the red component from RGB888.
     * @param rgb The rgb colour.
     * @return The red component.
     **/
    uint8_t getR(uint32_t rgb);

    /** @brief Extracts the green component from RGB888.
     * @param rgb The rgb colour.
     * @return The green component.
     **/
    uint8_t getG(uint32_t rgb);

    /** @brief Extracts the blue component from RGB888.
     * @param rgb The rgb colour.
     * @return The blue component.
    **/
    uint8_t getB(uint32_t rgb);

    /** @brief Sets the red component of RGB888.
     * @param value The RGB888 colour to be changed.
     * @param newValue The red component.
     **/
    uint32_t setR(uint32_t value, uint8_t newValue);

    /** @brief Sets the green component of RGB888.
     * @param value The RGB888 colour to be changed.
     * @param newValue The green component.
     **/
    uint32_t setG(uint32_t value, uint8_t newValue);

    /** @brief Sets the blue component of RGB888.
     * @param value The RGB888 colour to be changed.
     * @param newValue The blue component.
     **/
    uint32_t setB(uint32_t value, uint8_t newValue);

    /** @brief Changes the red component of RGB888.
     * @param value The RGB888 colour to be changed.
     * @param change The change of the red component.
     **/
    uint32_t changeR(uint32_t value, int16_t change);

    /** @brief Changes the green component of RGB888.
     * @param value The RGB888 colour to be changed.
     * @param change The change in the green component.
     **/
    uint32_t changeG(uint32_t value, int16_t change);

    /** @brief Changes the blue component of RGB888.
     * @param value The RGB888 colour to be changed.
     * @param change The change in the blue component.
     **/
    uint32_t changeB(uint32_t value, int16_t change);

    /** @brief Creates an RGB888 colour from its separate components.
     * @param r The red component.
     * @param g The green component.
     * @param b The blue component.
     * @return The resulting RGB888 colour.
     **/
    uint32_t fromRGB(uint8_t r, uint8_t g, uint8_t b);

    /** @brief Converts an RGB888 colour to an RGB565 colour.
     * @param rgb888 The RGB888 colour.
     * @return The resulting RGB565 colour.
     * @see rgb888to565
     **/
    uint16_t rgb888to565(uint32_t rgb888);

    /** @brief Converts an RGB565 colour to an RGB888 colour.
     * @param rgb565 The RGB565 colour.
     * @return The resulting RGB888 colour.
     **/
    uint32_t rgb565to888(uint16_t rgb565);
};

/* namespace Colours */