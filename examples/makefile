CC= g++
CXXFLAGS= -I/usr/include/freetype2 -I../include -D_LINUX
LINK= -lSDL2 -lvorbis -lvorbisfile -lopenal -ljpeg -lpng -lfreetype -lz -lsmeltext -lsmelt -lCxImage
LINK_FOLDER= -L../smelt/sdl -L../extensions

all:
	$(CC) smelt_test.cpp $(CXXFLAGS) $(LINK_FOLDER) $(LINK) -o smelt_test
clean:
	rm smelt_test