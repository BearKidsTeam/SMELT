CC= g++
CXXFLAGS= -c -g -O2 -std=c++11 -Wall -I/usr/include/SDL/ -I../../include -D_LINUX -fPIC
TARGET= libsmelt.a

all: objects-normal archive-normal

dumb: CXXFLAGS += -DENABLE_DUMB
dumb: TARGET= libsmelt-dumb.a
dumb: all

objects-normal: gfx inp sfx sys

gfx:
	$(CC) gfx_sdl.cpp $(CXXFLAGS)
sfx:
	$(CC) sfx_sdl.cpp $(CXXFLAGS)
inp:
	$(CC) inp_sdl.cpp $(CXXFLAGS)
sys:
	$(CC) sys_sdl.cpp $(CXXFLAGS)
archive-normal:
	$(AR) rcs $(TARGET) gfx_sdl.o sfx_sdl.o inp_sdl.o sys_sdl.o

clean:
	rm -f *.o
clean-all: clean
	rm *.a
