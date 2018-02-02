// -*- C++ -*-
/*
 * Simple MultimEdia LiTerator(SMELT)
 * by Chris Xiong 2015
 * Truetype font support header
 *
 * WARNING: This library is in development and interfaces would be very
 * unstable.
 * This library is developed for the BLR series games.
 */
#ifndef SMTTFONT_H
#define SMTTFONT_H
#include "smelt.hpp"
#include <cwchar>
#include <map>
#include <utility>
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H

#ifndef ALIGN_LEFT
#define ALIGN_LEFT 0
#define ALIGN_RIGHT 1
#define ALIGN_CENTER 2
#endif

struct _smTexState;
class _smTTChar;

class smTTFont
{
private:
	FT_Library ftlib;
	FT_Face ttface;
	wchar_t buf[1025];
	std::map<wchar_t,_smTTChar*> chars;
	std::vector<_smTexState*> textures;
	float w,h;
	int mx,my,texw,texh;
	unsigned _npot(unsigned x);
	std::pair<SMTEX,std::pair<int,int>> _allocate_char(int rw,int rh);
	static SMELT* sm;
public:
	~smTTFont();
	bool loadTTF(const char* path,int pt,int cachesize_x=16,int cachesize_y=16);
	bool loadTTFFromMemory(char* ptr,DWORD size,int pt,int cachesize_x=16,int cachesize_y=16);
	void releaseTTF();
	float getWidth(){return w;}
	float getHeight(){return h;}
	void updateString(const wchar_t *format,...);
	void render(float x,float y,float z,DWORD col,int align,float scalex=1,float scaley=1);
	DWORD getCacheSize();
	void clearCache();
	friend class _smTTChar;
};
#endif
