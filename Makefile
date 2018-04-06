CC=gcc
CFLAGS=-Wall -Werror -std=gnu++11
LIBS=

SRC=main.cpp Engine.cpp Network/UDP.cpp Units/LightUnit.cpp

default_target: all

all:
	$(CC) $(CFLAGS) $(LIBS) -o app $(SRC)