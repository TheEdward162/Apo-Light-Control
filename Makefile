CC=g++
CFLAGS=-Wall -Werror -std=gnu++11 -D_REENTRANT
PC_FLAGS=$(shell sdl2-config --cflags)
LIBS=-lpthread
PC_LIBS=$(shell sdl2-config --libs)

SRC=main.cpp Engine.cpp Network/*.cpp Unit/*.cpp Misc/*.cpp MZApi/*.cpp DisplayUtils/*.cpp
PACK_FILES=Makefile *.cpp *.h Network/* Unit/* Misc/* Misc/* MZApi/* DisplayUtils/* icons/* docs/Doxyfile docs/doxygen-css.css

default_target: all

.PHONY: all test docs

all:
	$(CC) $(CFLAGS) $(PC_FLAGS) -o app $(SRC) $(LIBS) $(PC_LIBS)

board:
	$(CC) $(CFLAGS) -DMZ_BOARD -o app $(SRC) $(LIBS)

test:
	./app "OMG room" icons/tux.ppm

grind:
	valgrind --leak-check=full --track-origins=yes ./app "OMG room" icons/tux.ppm

docs:
	doxygen Doxyfile

pack:
	tar -zcvf pack.tar.gz $(PACK_FILES)

zip:
	zip pack.zip $(PACK_FILES)

tar:
	tar -cvf pack.tar $(PACK_FILES)