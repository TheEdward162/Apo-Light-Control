#include <cmath>

#include "Screen.h"
#include "Colour.h"
#include "../Engine.h"

Screen::Screen(Display* display) {
	this->display = display;
}

Screen::~Screen() {}


void Screen::renderLine(int y, Screen::PLineElement element, int32_t lineColor) {
	if (lineColor >= 0)
		display->renderRectangle(0, y, Display::width, y + 16, (uint16_t) lineColor);

	if (element->alignRight) {
		element->renderSelf(display, Display::width, y);
	} else {
		element->renderSelf(display, 0, y);
	}
}

void Screen::renderLine(int y, PLineElementVector& elements, int32_t lineColor) {
	if (lineColor >= 0)
		display->renderRectangle(0, y, Display::width, y + 16, (uint16_t) lineColor);

	int leftX = 0;
	int rightX = 0;
	for (auto& lineElement : elements) {
		if (lineElement->alignRight) {
			rightX += lineElement->renderSelf(display, Display::width - rightX, y);
		} else {
			leftX += lineElement->renderSelf(display, leftX, y);
		}
	}
}

void Screen::renderNagivationLine(std::vector<std::pair<std::string, std::string>> uiInputPairs) {
	size_t x = 0;
	size_t y = Display::height - 16;

	display->renderRectangle(x, y, Display::width, Display::height, Colour::ORANGE);
	x += 2;

	for (std::pair<std::string, std::string> inputPair : uiInputPairs) {
		std::string key = std::get<0>(inputPair);
		if (Engine::uiIcons.count(key)) {
			std::string value = std::get<1>(inputPair);

			display->renderIcon(Engine::uiIcons[key].data(), x, y);
			x += 16 + 4;

			x += display->renderText(x, y, value, display->fgColour) + 4;
		}
	}
}

Screen::LineElement::LineElement(int margin, bool alignRight) : LineElement(margin, margin, alignRight) {}
Screen::LineElement::LineElement(int marginLeft, int marginRight, bool alignRight) {
	this->marginLeft = marginLeft;
	this->marginRight = marginRight;

	this->alignRight = alignRight;
}

Screen::SpaceLineElement::SpaceLineElement(int size, bool alignRight) : LineElement(size, 0, alignRight) {}
int Screen::SpaceLineElement::renderSelf(Display* display, int x, int y) {
	return marginLeft;
}

Screen::ColorSquareLineElement::ColorSquareLineElement(uint16_t color, int margin, bool alignRight) : ColorSquareLineElement(color, margin, margin, alignRight) {}
Screen::ColorSquareLineElement::ColorSquareLineElement(uint16_t color, int marginLeft, int marginRight, bool alignRight) : LineElement(marginLeft, marginRight, alignRight) {
	this->color = color;
}
int Screen::ColorSquareLineElement::renderSelf(Display* display, int x, int y) {
	int leftX = x + marginLeft;
	if (alignRight)
		leftX = x - marginRight - 16;

	display->renderColourSquare(leftX, y, color);
	return 16 + marginLeft + marginRight;
}

Screen::TextLineElement::TextLineElement(std::string text, uint16_t color, int margin, bool alignRight) : TextLineElement(text, color, margin, margin, alignRight) {}
Screen::TextLineElement::TextLineElement(std::string text, uint16_t color, int marginLeft, int marginRight, bool alignRight) : LineElement(marginLeft, marginRight, alignRight) {
	this->text = text;
	this->color = color;
}
int Screen::TextLineElement::renderSelf(Display* display, int x, int y) {
	int leftX = x + marginLeft;
	if (alignRight)
		leftX = x - marginRight - display->textWidth(text);
	
	return marginLeft + display->renderText(leftX, y, text, color) + marginRight;
}

Screen::IconLineElement::IconLineElement(uint16_t* pIcon, int scaleExponent, int margin, bool alignRight) : IconLineElement(pIcon, scaleExponent, margin, margin, alignRight) {}
Screen::IconLineElement::IconLineElement(uint16_t* pIcon, int scaleExponent, int marginLeft, int marginRight, bool alignRight) : LineElement(marginLeft, marginRight, alignRight) {
	this->pIcon = pIcon;
	this->scaleExponent = scaleExponent;
}
int Screen::IconLineElement::renderSelf(Display* display, int x, int y) {
	int leftX = x + marginLeft;
	if (alignRight)
		leftX = x - marginRight - 16 * std::pow(2, scaleExponent);

	return marginLeft + display->renderIcon(pIcon, leftX, y, scaleExponent) + marginRight;
}