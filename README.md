# SMELT
_SMELT -- The engine behind BLR3+._

_Now has its own repo._

A bad, bad reference is now in the doc folder.

Currently only the SDL version is available.

A D3D version will be put into construction _soon_.

## Building
Building the SDL version of SMELT is now officially tested
on debian sid, debian jessie and Arch Linux.

Theoretically the SDL version should work on Windows
with little modification :)

Building dependencies on debian-based systems:

>libfreetype6-dev libopenal-dev libsdl2-dev libpng12-dev
>libjpeg62-turbo-dev libvorbis-dev libogg-dev

To build a module, just run make in the corresponding folder.

The example must be built last.

SMELT and applications that uses SMELT *REQUIRES* C++11 to build.

## TODOs:
* Example code.
* Better documentation.
* D3D version.
* Better tools?
