CC= g++
CXXFLAGS= -c -g -O2 -std=c++11 -Wall -I../include -I/usr/include/freetype2 -fPIC

all: objects archive clean

objects:
	$(CC) *.cpp $(CXXFLAGS)
archive:
	$(AR) rcs libsmeltext.a *.o
clean:
	if ( test smanim.o ); then rm *.o; fi
clean-all: clean
	rm *.a