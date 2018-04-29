//
// Created by klara on 21.4.18.
//

#pragma once

#include <vector>
#include <string>
#include <memory>

#include "../MZApi/Display.h"

class Display;

/**
	@defgroup screens Screens module
	@brief Screen base class and implementations.
*/

/**
	@addtogroup screens
	@{
*/

/** 
	@brief Class representing one screen.
*/
class Screen {
public:
	virtual ~Screen() = 0;

    /**
     * @brief Renders this screen to display.
     */
    virtual void renderScreen() = 0;

	/**
     * @brief Handles knob changes.
     */
    virtual void handleKnobChange(int8_t RGBDelta[3]) = 0;

	/**
     * @brief Handles knob presses.
     */
    virtual void handleKnobPress(bool RGBPressed[3]) = 0;

protected:
	/**
     * @brief Line element base class.
     */
	class LineElement {
	public:
		LineElement(int margin=0, bool alignRight=false);
		LineElement(int marginLeft, int marginRight, bool alignRight=false);

		/**
			@brief Renders itself at position x, y to display.
			@param[in] display Display to render to.
			@param[in] x X position to render to.
			@param[in] y Y position to render to.
			@return Number of pixels this elements takes.
		*/
		virtual int renderSelf(Display* display, int x, int y) = 0;
		/**
			@brief Wether to align left or right. 
		*/
		bool alignRight;

	protected:
		/**
			@brief Left margin of this element. Can be negative.
		*/
		int marginLeft;
		/**
			@brief Right margin of this element. Can be negative.
		*/
		int marginRight;
	};
	/** @brief Empty space */
	class SpaceLineElement : public LineElement {
	public:
		SpaceLineElement(int size, bool alignRight=false);

		int renderSelf(Display* display, int x, int y);
	};
	/** @brief Color square */
	class ColorSquareLineElement : public LineElement {
	public:
		ColorSquareLineElement(uint16_t color=0, int margin=0, bool alignRight=false);
		ColorSquareLineElement(uint16_t color, int marginLeft, int marginRight, bool alignRight=false);

		int renderSelf(Display* display, int x, int y);
	protected:
		/**
			@brief Color of the square.
		*/
		uint16_t color;
	};
	/** @brief Text */
	class TextLineElement : public LineElement {
	public:
		TextLineElement(std::string text="", uint16_t color=0, int margin=0, bool alignRight=false);
		TextLineElement(std::string text, uint16_t color, int marginLeft, int marginRight, bool alignRight=false);

		int renderSelf(Display* display, int x, int y);
	protected:
		/**
			@brief The text.
		*/
		std::string text;
		/**
			@brief Color of the text.
		*/
		uint16_t color;
	};
	/** @brief Scaled icon */
	class IconLineElement : public LineElement {
	public:
		IconLineElement(uint16_t* pIcon=NULL, int scaleExponent=0, int margin=0, bool alignRight=false);
		IconLineElement(uint16_t* pIcon, int scaleExponent, int marginLeft, int marginRight, bool alignRight=false);

		int renderSelf(Display* display, int x, int y);
	protected:
	 	/**
		 @brief Pointer to the icon buffer.
		*/
		uint16_t* pIcon;
		/**
			@brief Exponent to scale icon by.
			@see Display::renderIcon
		*/
		int scaleExponent;
	};
	typedef std::unique_ptr<LineElement> PLineElement;
	typedef std::vector<Screen::PLineElement> PLineElementVector;

	Screen(Display* display);

    Display* display;
    size_t selected = 0; /*< Currently selected line. */

	/**
		@brief Renders line containing only one element.
		@param[in] y Y position to render the line at.
		@param[in] element Element to render in this line.
		@param[in] lineColor Color of this line. Pass -1 to not draw any line background.
	*/
	void renderLine(int y, Screen::PLineElement element, int32_t lineColor=-1);

	/**
		@brief Renders line containing elements.
		@param[in] y Y position to render the line at.
		@param[in] elements Elements to render in this line.
		@param[in] lineColor Color of this line. Pass -1 to not draw any line background.
	*/
	void renderLine(int y, Screen::PLineElementVector& elements, int32_t lineColor=-1);

	/**
		@brief Renders navigation line.
		@param[in] uiInputPairs Vector of string pairs (icon_name, text) to render.
		@see Engine::uiIcons
	*/
	void renderNagivationLine(std::vector<std::pair<std::string, std::string>> uiInputPairs);
};
/** @} */