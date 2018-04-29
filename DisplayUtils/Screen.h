//
// Created by klara on 21.4.18.
//

#pragma once

#include <vector>
#include <string>
#include <memory>

#include "../MZApi/Display.h"

class Display;

class Screen {
public:
	virtual ~Screen() = 0;

    // pure virtual function
    virtual void renderScreen() = 0;

    virtual void handleKnobChange(int8_t RGBDelta[3]) = 0;
    virtual void handleKnobPress(bool RGBPressed[3]) = 0;

protected:
	class LineElement {
	public:
		LineElement(int margin=0, bool alignRight=false);
		LineElement(int marginLeft, int marginRight, bool alignRight=false);

		virtual int renderSelf(Display* display, int x, int y) = 0;
		bool alignRight;

	protected:
		int marginLeft;
		int marginRight;
	};
	class SpaceLineElement : public LineElement {
	public:
		SpaceLineElement(int size, bool alignRight=false);

		int renderSelf(Display* display, int x, int y);
	};
	class ColorSquareLineElement : public LineElement {
	public:
		ColorSquareLineElement(uint16_t color=0, int margin=0, bool alignRight=false);
		ColorSquareLineElement(uint16_t color, int marginLeft, int marginRight, bool alignRight=false);

		int renderSelf(Display* display, int x, int y);
	protected:
		uint16_t color;
	};
	class TextLineElement : public LineElement {
	public:
		TextLineElement(std::string text="", uint16_t color=0, int margin=0, bool alignRight=false);
		TextLineElement(std::string text, uint16_t color, int marginLeft, int marginRight, bool alignRight=false);

		int renderSelf(Display* display, int x, int y);
	protected:
		std::string text;
		uint16_t color;
	};
	class IconLineElement : public LineElement {
	public:
		IconLineElement(uint16_t* pIcon=NULL, int scaleExponent=0, int margin=0, bool alignRight=false);
		IconLineElement(uint16_t* pIcon, int scaleExponent, int marginLeft, int marginRight, bool alignRight=false);

		int renderSelf(Display* display, int x, int y);
	protected:
		uint16_t* pIcon;
		int scaleExponent;
	};
	typedef std::unique_ptr<LineElement> PLineElement;
	typedef std::vector<Screen::PLineElement> PLineElementVector;

	Screen(Display* display);

    Display* display;
    size_t selected = 0;

	void renderLine(int y, Screen::PLineElement element, int32_t lineColor=-1);
	void renderLine(int y, Screen::PLineElementVector& elements, int32_t lineColor=-1);
	void renderNagivationLine(std::vector<std::pair<std::string, std::string>> uiInputPairs);
};