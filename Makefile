CC=g++
CFLAGS=-Wall -Werror -std=gnu++11 -D_REENTRANT
PC_FLAGS=-I/usr/include/SDL2
LIBS=-lpthread
PC_LIBS=-lSDL2

SRC=main.cpp Engine.cpp Network/*.cpp Unit/*.cpp Misc/*.cpp MZApi/*.cpp DisplayUtils/*.cpp

default_target: all

all:
	$(CC) $(CFLAGS) $(PC_FLAGS) $(LIBS) $(PC_LIBS) -o app $(SRC)

board:
	$(CC) $(CFLAGS) -DMZ_BOARD $(LIBS) -o app $(SRC)

test:
	./app "OMG room" test.ppm

pack:
	tar -zcvf pack.tar.gz Makefile *.cpp *.h Network/* Unit/* Misc/* Misc/* MZApi/* DsiplayUtils/* icons/*