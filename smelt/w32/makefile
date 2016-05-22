CC= i686-w64-mingw32-g++
CXXFLAGS= -c -g -O2 -std=c++11 -Wall -I../include -D_WIN32 -fPIC

all: objects-normal archive-normal

dumb: objects-dumb archive-dumb

objects-normal: gfx inp sfx sys

objects-dumb: gfx inp sfx-dumb sys

gfx:
	$(CC) gfx_d3d.cpp $(CXXFLAGS)
sfx:
	$(CC) sfx_w32.cpp $(CXXFLAGS)
sfx-dumb:
	$(CC) sfx_dumb.cpp $(CXXFLAGS)
inp:
	$(CC) inp_w32.cpp $(CXXFLAGS)
sys:
	$(CC) sys_w32.cpp $(CXXFLAGS)
archive-normal:
	$(AR) rcs libsmelt.a gfx_d3d.o sfx_w32.o inp_w32.o sys_w32.o
archive-dumb:
	$(AR) rcs libsmelt-dumb.a gfx_d3d.o sfx_dumb.o inp_w32.o sys_w32.o
clean:
	rm -f *.o
clean-all: clean
	rm *.a