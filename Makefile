CC=g++
CFLAGS=-Wall -Werror -std=gnu++11
LIBS=-D_REENTRANT -lpthread

SRC=main.cpp Engine.cpp Network/*.cpp Unit/*.cpp Misc/*.cpp MZApi/*.cpp

default_target: all

all:
	$(CC) $(CFLAGS) $(LIBS) -o app $(SRC)

btest: all
	./app "OMG room" test.ppm

test:
	./app "OMG room" test.ppm

pack:
	tar -zcvf pack.tar.gz Makefile *.cpp *.h Network/* Unit/* Misc/* Misc/* MZApi/* test.ppm