//
// Created by klara on 14.4.18.
//

#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

#include "Display.h"
#include "Mapper.h"
#include "DeviceInput.h"
#include "../DisplayUtils/font_types.h"
#include "../MZApi/MZRegisters.h"
#include "../Unit/LightUnit.h"
#include "../DisplayUtils/Screen.h"

#ifndef MZ_BOARD
#include <SDL2/SDL.h>
#endif

class Screen;

/**
	@addtogroup unit
	@{
*/

/**
	@brief A class that handles the (one-way) interaction with the device display
	and provides methods for rendering shapes, text, and other.
**/
class Display {
public:
	/**
		@brief The display width
	*/
	static const size_t width = 480;
	/**
		@brief The display height
	*/
	static const size_t height = 320;

	/** @brief The display constructor taking colours and fonts as parameters.
     * @param bgColour The colour used for background.
     * @param fgColour The colour used for foreground.
     * @param highlightColour The colour used for highlighted items, such as the sleected ones.
     * @param font The font to be used for displayed text.
     **/
    Display(uint16_t bgColour, uint16_t fgColour, uint16_t highlightColour, font_descriptor_t font, DeviceInput* deviceInput);

	/** @brief The display destructor.
     **/
    ~Display();
    
	/**
		@brief Current theme text colour.
	*/
	uint16_t fgColour;
	/**
		@brief Current theme background colour.
	*/
	uint16_t bgColour;
	/**
		@brief Current theme selected background colour.
	*/
	uint16_t selectColour;

	/**
		@brief The maximum number of lines that fit on the display.
	*/
    size_t lineMax;

	/** @brief Reacts to input from the device.
     **/
    void handleInput();

    /** @brief Changes the display screen.
     * @param newScreen The new screen.
     **/
	void switchScreen(Screen* newScreen);

    /** @brief Returns to the previous screen.
     * @param keepAlive Whether the current screen should be preserved so that it can be returned to later.
     * @return
     **/
	bool toPreviousScreen(bool keepAlive=false);

	/** @brief Sets the base colours for the display - background, foreground and highlight.
     * @param bgColour The colour used for background.
     * @param fgColour The colour used for foreground.
     * @param highlightColour The colour used for highlighted items, such as the sleected ones.
     * @return True if there was a previous screen, false otherwise.
     **/
    void setColours(uint16_t bgColour, uint16_t fgColour, uint16_t highlightColour);

	/** @brief Sets the font for the display.
     * @param font The font to be used for displayed text.
     **/
    void setFont(font_descriptor_t font);

	/** @brief Calculates text width in pixels.
     * @param text The text to calculate width for.
     * @return Width of the passed text in pixels.
     **/
	size_t textWidth(std::string& text);

	/** @brief Sets the whole display to one colour.
     * @param colour The colour used as background.
     **/
	void clearScreen(uint16_t colour);

	/** @brief Sets one pixel to a given colour.
     * @param x The pixel x coordinate.
     * @param y The pixel y coordinate.
     * @param colour The colour of the pixel.
     **/
	void setPixel(int x, int y, uint16_t colour);

	/** @brief Renders an axis-aligned rectangle with given corner points in a given colour.
     * @param left The x coordinate of the left edge.
     * @param top The y coordinate of the top edge.
     * @param right The x cooridnate of the right edge.
     * @param bottom The y coordinate of the bottom edgge.
     * @param colour The colour of the rectangle.
     **/
	void renderRectangle(int left, int top, int right, int bottom, uint16_t colour);

	/** @brief Renders an axis-aligned rectangle with given position in a given colour.
     * @param topX The x coordinate of the left edge.
     * @param topY The y coordinate of the top edge.
     * @param colour The colour of the rectangle.
     **/
    void renderColourSquare(int topX, int topY, uint16_t colour);

	/** @brief Renders an axis-aligned text-line starting at a given position in a given colour.
     * @param topX The x coordinate of the top-left corner.
     * @param topY The y coordinate of the top-left corner.
     * @param text The text to be rendered.
     * @param colour The colour of the text.
	 * @return Width rendered text in pixels.
     **/
    size_t renderText(int topX, int topY, std::string text, uint16_t colour);

	/** @brief Renders an icon starting at a given position.
	 * @param buffer The 16x16 image to be rendered.
     * @param topX The x coordinate of the top-left corner.
     * @param topY The y coordinate of the top-left corner.
	 * @param exponent The exponent to use for scaling by powers of two.
	 * @return Width rendered icon in pixels.
     **/
    size_t renderIcon(uint16_t *buffer, int topX, int topY, int exponent=0);

	/** @brief Renders the display buffer on the device.
     **/
    void redraw();

private:
	uint16_t frameBuffer[height][width];
    font_descriptor_t font;
    DeviceInput* deviceInput = NULL;

#ifndef MZ_BOARD
	SDL_Window* sdl_win = NULL;
	int8_t sdl_knobDeltas[3] = {0, 0, 0};
	bool sdl_knobPresses[3] = {false, false, false};
#endif
	Screen* currentScreen = NULL;
	Screen* previousScreen = NULL;

    void parlcd_write_cmd(uint16_t cmd);
    void parlcd_write_data(uint16_t data);
    size_t renderCharacter(char character, int topX, int topY, uint16_t colour);

    Mapper mapper = Mapper(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE);

	bool checkBounds(int* x, int* y);

    bool getBit(uint16_t bits, int position);
};
/** @} */