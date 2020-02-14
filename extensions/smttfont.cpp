// -*- C++ -*-
/*
 * Simple MultimEdia LiTerator(SMELT)
 * by Chris Xiong 2015
 * Truetype font support implementation
 *
 * WARNING: This library is in development and interfaces would be very
 * unstable.
 * This library is developed for the BLR series games.
 */
#include "smttfont.hpp"

struct _smTexState
{
	SMTEX tex;
	int cx,cy;
	int mh;
};

class _smTTChar
{
private:
	smQuad quad;
	int rw,rh,_w,_h,xofs,yofs;
	static SMELT *sm;
public:
	~_smTTChar();
	float w(){return (float)_w;}
	float h(){return (float)_h;}
	void free();
	bool setChar(wchar_t c,FT_Face ttface,smTTFont* par);
	void render(float x,float y,float z,DWORD col,float scalex,float scaley);
};

SMELT *_smTTChar::sm=NULL;
SMELT *smTTFont::sm=NULL;

_smTTChar::~_smTTChar(){free();}
void _smTTChar::free(){sm->smRelease();}
bool _smTTChar::setChar(wchar_t c,FT_Face ttface,smTTFont* par)
{
	sm=smGetInterface(SMELT_APILEVEL);
	FT_GlyphSlot slot=ttface->glyph;
	FT_UInt glyph_index=FT_Get_Char_Index(ttface,c);
	if(FT_Load_Glyph(ttface,glyph_index,FT_LOAD_DEFAULT))return false;
	if(FT_Render_Glyph(ttface->glyph,FT_RENDER_MODE_NORMAL))return false;
	_w=slot->advance.x>>6;_h=3*slot->bitmap.rows-2*slot->bitmap_top;
	rw=slot->bitmap.width;rh=slot->bitmap.rows;
	xofs=slot->bitmap_left;yofs=slot->bitmap.rows-slot->bitmap_top;
	std::pair<SMTEX,std::pair<int,int>> loc=par->_allocate_char(rw?rw:1,rh?rh:1);
	int dx=loc.second.first,dy=loc.second.second;
	quad.tex=loc.first;
	DWORD* px=sm->smTextureLock(quad.tex,dx,dy,rw?rw:1,rh?rh:1,false);
	memset(px,0,sizeof(DWORD)*(rw?rw:1)*(rh?rh:1));
	int ptr=0;
	for(int i=0;i<rh;++i)
	for(int j=0;j<rw;++j)
	{
#ifdef _D3D
		px[i*rw+j]=ARGB(slot->bitmap.buffer[ptr],255,255,255);
#else
		px[(rh-i-1)*rw+j]=ARGB(slot->bitmap.buffer[ptr],255,255,255);
#endif
		++ptr;
	}
	sm->smTexutreUnlock(quad.tex);
	quad.blend=BLEND_ALPHABLEND;
	quad.v[0].tx=1.*dx/par->texw;quad.v[0].ty=1.*dy/par->texh;
	quad.v[1].tx=1.*(rw+dx)/par->texw;quad.v[1].ty=1.*dy/par->texh;
	quad.v[2].tx=1.*(rw+dx)/par->texw;quad.v[2].ty=1.*(rh+dy)/par->texh;
	quad.v[3].tx=1.*dx/par->texw;quad.v[3].ty=1.*(rh+dy)/par->texh;
	return true;
}
void _smTTChar::render(float x,float y,float z,DWORD col,float scalex,float scaley)
{
	for(int i=0;i<4;++i)quad.v[i].col=col,quad.v[i].z=z;
	quad.v[0].x=x+xofs*scalex;quad.v[0].y=y+(yofs-rh)*scaley;
	quad.v[1].x=x+(rw+xofs)*scalex;quad.v[1].y=y+(yofs-rh)*scaley;
	quad.v[2].x=x+(rw+xofs)*scalex;quad.v[2].y=y+yofs*scaley;
	quad.v[3].x=x+xofs*scalex;quad.v[3].y=y+yofs*scaley;
	sm->smRenderQuad(&quad);
}

smTTFont::~smTTFont(){}
unsigned smTTFont::_npot(unsigned x)
{
	--x;
	for(unsigned i=1;i<sizeof(unsigned)*8;i<<=1)x|=x>>i;
	return x+1;
}
std::pair<SMTEX,std::pair<int,int>> smTTFont::_allocate_char(int rw,int rh)
{
	std::pair<SMTEX,std::pair<int,int>> ret;
	ret.first=0;ret.second.first=ret.second.second=-1;
	if(~texw&&(rw>texw||rh>texh))return ret;//allocation failure
	for(_smTexState* i:textures)
		if(i->cx+rw<=texw&&i->cy+rh<=texh)
		{
			ret.first=i->tex;
			ret.second.first=i->cx;
			ret.second.second=i->cy;
			i->cx+=rw;
			if(rh>i->mh)i->mh=rh;
			break;
		}
		else if(i->cy+i->mh+rh<=texh)
		{
			i->cx=rw;i->cy+=i->mh;
			i->mh=rh;
			ret.first=i->tex;
			ret.second.first=0;
			ret.second.second=i->cy;
			break;
		}
	if(ret.first)return ret;
	if(!~texw)
	{
		int rd=rw>rh?rw:rh;
		texw=_npot(mx*rd);
		texh=_npot(my*rd);
	}
	_smTexState* ptex=new _smTexState;
	ptex->cx=ptex->cy=ptex->mh=0;
	ptex->tex=sm->smTextureCreate(texw,texh);
	ret.first=ptex->tex;
	ret.second.first=ret.second.second=0;
	ptex->cx+=rw;
	textures.push_back(ptex);
	return ret;
}
bool smTTFont::loadTTF(const char* path,int pt,int cachesize_x,int cachesize_y)
{
	sm=smGetInterface(SMELT_APILEVEL);
	if(FT_Init_FreeType(&ftlib))return false;
	if(FT_New_Face(ftlib,path,0,&ttface))return false;
	if(FT_Set_Char_Size(ttface,0,pt*64,96,96))return false;
	chars.clear();textures.clear();texw=texh=-1;
	mx=cachesize_x;my=cachesize_y;
	return true;
}
bool smTTFont::loadTTFFromMemory(char* ptr,DWORD size,int pt,int cachesize_x,int cachesize_y)
{
	sm=smGetInterface(SMELT_APILEVEL);
	if(FT_Init_FreeType(&ftlib))return false;
	if(FT_New_Memory_Face(ftlib,(const FT_Byte*)ptr,(FT_Long)size,0,&ttface))return false;
	if(FT_Set_Char_Size(ttface,0,pt*64,96,96))return false;
	chars.clear();textures.clear();texw=texh=-1;
	mx=cachesize_x;my=cachesize_y;
	return true;
}
void smTTFont::releaseTTF()
{
	clearCache();
	FT_Done_Face(ttface);
	FT_Done_FreeType(ftlib);
	sm->smRelease();
}
void smTTFont::updateString(const wchar_t *format,...)
{
	memset(buf,0,sizeof(buf));
	va_list vl;
	va_start(vl,format);
	vswprintf(buf,1024,format,vl);
	va_end(vl);
	buf[1024]='\0';
	w=h=0;float lh=0;
	for(int i=0;buf[i]!='\0';++i)
	{
		if(chars.find(buf[i])==chars.end()&&buf[i]!=L'\n')
		{
			chars[buf[i]]=new _smTTChar();
			if(!chars[buf[i]]->setChar(buf[i],ttface,this))
			{
				delete chars[buf[i]];
				chars.erase(chars.find(buf[i]));
			}
		}
		if(chars.find(buf[i])!=chars.end())
		{
			w+=chars[buf[i]]->w();
			if(chars[buf[i]]->h()>lh)lh=chars[buf[i]]->h();
		}
		if(buf[i]==L'\n')h+=lh,lh=0;
	}
	h+=lh;
}
void smTTFont::render(float x,float y,float z,DWORD col,int align,float scalex,float scaley)
{
	float curx,cury,lh;
	if(align==ALIGN_LEFT)
	{
		curx=x;cury=y;lh=0;
		for(int i=0;buf[i]!=L'\0';++i)
			if(chars.find(buf[i])!=chars.end())
				if(chars[buf[i]]->h()>lh)lh=chars[buf[i]]->h();
		for(int i=0;buf[i]!=L'\0';++i)
		{
			if(buf[i]!=L'\n')
			{
				if(chars.find(buf[i])!=chars.end())
				{
					chars[buf[i]]->render(curx,cury,z,col,scalex,scaley);
					curx+=chars[buf[i]]->w()*scalex;
				}
			}
			else cury+=lh*scaley,curx=x;
		}
	}
	if(align==ALIGN_RIGHT)
	{
		curx=x;cury=y;lh=0;
		for(int i=wcslen(buf)-1;i>=0;--i)
			if(chars.find(buf[i])!=chars.end())
				if(chars[buf[i]]->h()>lh)lh=chars[buf[i]]->h();
		for(int i=wcslen(buf)-1;i>=0;--i)
		{
			if(buf[i]!=L'\n')
			{
				if(chars.find(buf[i])!=chars.end())
				{
					chars[buf[i]]->render(curx,cury,z,col,scalex,scaley);
					if(i&&buf[i-1]!=L'\n')curx-=chars[buf[i-1]]->w()*scalex;
				}
			}
			else cury-=lh*scaley,curx=x;
		}
	}
}
DWORD smTTFont::getCacheSize(){return chars.size();}
void smTTFont::clearCache()
{
	for(std::map<wchar_t,_smTTChar*>::iterator i=chars.begin();i!=chars.end();++i)
	delete i->second;
	for(_smTexState* i:textures)
	{
		sm->smTextureFree(i->tex);
		delete i;
	}
	textures.clear();
	texw=texh=-1;
	chars.clear();
}
