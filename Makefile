CC=g++
CFLAGS=-Wall -Werror -std=gnu++11
LIBS=-D_REENTRANT -lpthread

SRC=main.cpp Engine.cpp Network/*.cpp Unit/*.cpp Misc/*.cpp MZApi/*.cpp DisplayUtils/*.cpp

default_target: all

all:
	$(CC) $(CFLAGS) $(LIBS) -o app $(SRC)

board:
	$(CC) $(CFLAGS) $(LIBS) -DMZ_BOARD -o app $(SRC)

test:
	./app "OMG room" test.ppm

pack:
	tar -zcvf pack.tar.gz Makefile *.cpp *.h Network/* Unit/* Misc/* Misc/* MZApi/* DsiplayUtils/* icons/*