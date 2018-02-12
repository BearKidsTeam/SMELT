CC= g++
CXXFLAGS= -I/usr/include/freetype2 -I../include -D_LINUX -g
LINK= -lSDL2 -lvorbis -lvorbisfile -lopenal -ljpeg -lpng -lfreetype -lz -lsmeltext -lsmelt -lCxImage
LINK_GLFW= -lglfw -lGLEW -lGL -lvorbis -lvorbisfile -lopenal -ljpeg -lpng -lfreetype -lz -lsmeltext -lsmelt -lCxImage
LINK_FOLDER= -L../smelt/sdl -L../extensions
LINK_GLFW_FOLDER= -L../smelt/glfw -L../extensions

all: sdl glfw

sdl:
	$(CC) smelt_test.cpp $(CXXFLAGS) $(LINK_FOLDER) $(LINK) -o smelt_test
clean:
	rm smelt_test
glfw:
	$(CC) smelt_test.cpp $(CXXFLAGS) $(LINK_GLFW_FOLDER) $(LINK_GLFW) -o smelt_test_glfw
