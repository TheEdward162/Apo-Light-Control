#include "Screen.h"
#include "Colour.h"

Screen::~Screen() {}

void Screen::renderNagivationLine() {
	//TODO: Implement
	display->renderRectangle(0, HEIGHT - 16, WIDTH, HEIGHT, Colour::ORANGE);
}