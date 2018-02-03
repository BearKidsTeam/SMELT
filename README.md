# SMELT
_SMELT -- The engine behind BLR3+._

_Now has its own repo._

A ~~totally useless~~ reference is now in the doc folder.

A D3D version will be put into construction _soon_.

## Building
Building the SDL/GLFW version of SMELT is now officially tested
on debian sid, debian jessie, Arch Linux and Gentoo Linux.

Theoretically both versions should work on Windows
with little modification :)

Building dependencies on debian-based systems:

>libfreetype6-dev libopenal-dev libsdl2-dev libpng-dev
>libjpeg62-turbo-dev libvorbis-dev libogg-dev

Now there's also an port that uses GLFW instead of SDL2. 
Minor behavior differences exist between the two versions.

Additional building dependencies for the GLFW version on a debian-based system:

>libglfw3-dev libglew-dev

Now there's a OpenGL 3.2+ port for the GLFW version which can be found
in the `glfw_m` folder. It is experimental and buggy. Use with caution.

To build a module, just run make in the corresponding folder.
Please note that CxImage is also a module.

The example must be built last.

SMELT and applications that uses SMELT *REQUIRES* C++11 to build.

## Using SMELT:
Till now SMELT can only be built as a static library, which may make using SMELT
a bit clumsy...

0. Build SMELT. Running "make" should do the trick for you. Sorry, MSVC users!
1. Add the include folder in the source tree to the C++ header search folder of you project.
2. Add the libraries dependencies. Library dependencies are listed as following:

for the SDL2 version:

> -lsmeltext -lsmelt[-dumb] -lCxImage [-lvorbis -lvorbisfile -lopenal] -ljpeg -lpng -lSDL2

for the GLFW version:

> -lsmeltext -lsmelt[-dumb] -lCxImage [-lvorbis -lvorbisfile -lopenal] -ljpeg -lpng -lglfw -lGLEW -lGL

...and start coding!

## Customizing SMELT
Each implementation of SMELT has a file called smelt_config.hpp in its folder.
Modifying this file allows you to customize the functionality of SMELT. See the
comments in the file for details.

## TODOs:
* Example code.
* Better documentation.
* Sane way to build.
* Unify OpenGL versions.
* ~~D3D version.~~
* Better tools?
