CC=g++
CFLAGS=-Wall -Werror -std=gnu++11 -D_REENTRANT
PC_FLAGS=$(shell sdl2-config --cflags)
LIBS=-lpthread
PC_LIBS=$(shell sdl2-config --libs)

SRC=main.cpp Engine.cpp Network/*.cpp Unit/*.cpp Misc/*.cpp MZApi/*.cpp DisplayUtils/*.cpp

default_target: all

all:
	$(CC) $(CFLAGS) $(PC_FLAGS) $(LIBS) $(PC_LIBS) -o app $(SRC)

board:
	$(CC) $(CFLAGS) -DMZ_BOARD $(LIBS) -o app $(SRC)

test:
	./app "OMG room" icons/1.ppm

pack:
	tar -zcvf pack.tar.gz Makefile *.cpp *.h Network/* Unit/* Misc/* Misc/* MZApi/* DisplayUtils/* icons/*

zip:
	zip pack.zip Makefile *.cpp *.h Network/* Unit/* Misc/* Misc/* MZApi/* DisplayUtils/* icons/*

tar:
	tar -cvf pack.tar Makefile *.cpp *.h Network/* Unit/* Misc/* Misc/* MZApi/* DisplayUtils/* icons/*