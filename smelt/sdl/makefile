CC= g++
CXXFLAGS= -c -g -std=c++11 -Wall -I/usr/include/SDL/ -I../include -D_LINUX

all: objects-normal archive-normal

dumb: objects-dumb archive-dumb

objects-normal: gfx inp sfx sys

objects-dumb: gfx inp sfx-dumb sys

gfx:
	$(CC) gfx_sdl.cpp $(CXXFLAGS)
sfx:
	$(CC) sfx_sdl.cpp $(CXXFLAGS)
sfx-dumb:
	$(CC) sfx_dumb.cpp $(CXXFLAGS)
inp:
	$(CC) inp_sdl.cpp $(CXXFLAGS)
sys:
	$(CC) sys_sdl.cpp $(CXXFLAGS)
archive-normal:
	$(AR) rcs libsmelt.a gfx_sdl.o sfx_sdl.o inp_sdl.o sys_sdl.o
archive-dumb:
	$(AR) rcs libsmelt-dumb.a gfx_sdl.o sfx_dumb.o inp_sdl.o sys_sdl.o
clean:
	rm -f *.o
clean-all: clean
	rm *.a