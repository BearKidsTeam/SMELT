CC= g++
CXXFLAGS+= -c -g -O2 -std=c++11 -Wall -I../../include -D_LINUX -fPIC
TARGET= libsmelt.a

all: objects-normal archive-normal

dumb: CXXFLAGS += -DENABLE_DUMB
dumb: TARGET= libsmelt-dumb.a
dumb: all

objects-normal: gfx inp sfx sys

gfx:
	$(CC) gfx_glfw.cpp $(CXXFLAGS)
sfx:
	$(CC) sfx_oal.cpp $(CXXFLAGS)
inp:
	$(CC) inp_glfw.cpp $(CXXFLAGS)
sys:
	$(CC) sys_glfw.cpp $(CXXFLAGS)
archive-normal:
	$(AR) rcs $(TARGET) gfx_glfw.o sfx_oal.o inp_glfw.o sys_glfw.o

clean:
	rm -f *.o
clean-all: clean
	rm *.a
