CC=g++
CFLAGS=-Wall -Werror -std=gnu++11 -D_REENTRANT
PC_FLAGS=$(shell sdl2-config --cflags)
LIBS=-lpthread
PC_LIBS=$(shell sdl2-config --libs)

SRC=main.cpp Engine.cpp Network/*.cpp Unit/*.cpp Misc/*.cpp MZApi/*.cpp DisplayUtils/*.cpp

default_target: all

all:
	$(CC) $(CFLAGS) $(PC_FLAGS) -o app $(SRC) $(LIBS) $(PC_LIBS)

board:
	$(CC) $(CFLAGS) -DMZ_BOARD -o app $(SRC) $(LIBS)

test:
	./app "OMG room" icons/tux.ppm

grind:
	valgrind --leak-check=full --track-origins=yes ./app "OMG room" icons/tux.ppm

pack:
	tar -zcvf pack.tar.gz Makefile *.cpp *.h Network/* Unit/* Misc/* Misc/* MZApi/* DisplayUtils/* icons/*

zip:
	zip pack.zip Makefile *.cpp *.h Network/* Unit/* Misc/* Misc/* MZApi/* DisplayUtils/* icons/*

tar:
	tar -cvf pack.tar Makefile *.cpp *.h Network/* Unit/* Misc/* Misc/* MZApi/* DisplayUtils/* icons/*