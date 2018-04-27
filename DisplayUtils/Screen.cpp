#include "Screen.h"
#include "Colour.h"

Screen::Screen(Display* display) {
	this->display = display;
}

Screen::~Screen() {}

void Screen::renderNagivationLine() {
	//TODO: Implement
	display->renderRectangle(0, Display::height - 16, Display::width, Display::height, Colour::ORANGE);
}