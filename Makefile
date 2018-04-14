CC=g++
CFLAGS=-Wall -Werror -std=gnu++11
LIBS=-D_REENTRANT -lpthread

SRC=main.cpp Engine.cpp Network/NetworkHandler.cpp Unit/LightUnit.cpp Misc/IOTools.cpp MZApi/DeviceInput.cpp

default_target: all

all:
	$(CC) $(CFLAGS) $(LIBS) -o app $(SRC)

test: all
	./app "OMG room" test.ppm