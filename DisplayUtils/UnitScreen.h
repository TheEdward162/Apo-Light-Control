//
// Created by klara on 21.4.18.
//

#pragma once

#include "Screen.h"

#define CEILEING_R 2;
#define CEILEING_G 3;
#define CEILEING_B 4;
#define WALL_R 6;
#define WALL_G 7;
#define WALL_B 8;


class UnitScreen: public Screen {
public:
    UnitScreen(Display* display, LightUnit& unit);

    void renderScreen();

private:
    LightUnit unit;

};