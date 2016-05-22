// -*- C++ -*-
/*
 * Simple MultimEdia LiTerator(SMELT)
 * by Chris Xiong 2015
 * Internal header for Windows version
 *
 * WARNING: This library is in development and interfaces would be very
 * unstable.
 *
 */
#ifndef SMELT_INTERNAL_H
#define SMELT_INTERNAL_H

#include "../../include/smelt.hpp"
#include "smelt_config.hpp"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cstdarg>
#include <d3d9.h>
#include <d3dx9.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>
#ifdef _WIN32
#include <intrin.h>
#endif

#define xstr(s) str(s)
#define str(s) #s
#define SLINE xstr(__LINE__)

#define D3DFVF_SMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

class TRenderTargetList
{
public:
	int w,h,ms;
	IDirect3DTexture9* tex;
	IDirect3DSurface9*	pDepth;
	TRenderTargetList *next;
};
class TTextureList
{
public:
	SMTEX tex;
	int w,h;
	TTextureList *next;
};
class TInputEventList
{
public:
	smInpEvent e;
	TInputEventList *next;
};

class SMELT_IMPL:public SMELT
{
public:
	virtual void smRelease();
	virtual bool smInit();
	virtual void smFinale();
	virtual void smMainLoop();
	virtual void smUpdateFunc(smHook func);
	virtual void smUpdateFunc(smHandler* h);
	virtual void smUnFocFunc(smHook func);
	virtual void smUnFocFunc(smHandler* h);
	virtual void smFocFunc(smHook func);
	virtual void smFocFunc(smHandler* h);
	virtual void smQuitFunc(smHook func);
	virtual void smQuitFunc(smHandler* h);
	virtual void smWinTitle(const char* title);
	virtual bool smIsActive();
	virtual void smNoSuspend(bool para);
	virtual void smVidMode(int resX,int resY,bool _windowed);
	virtual void smLogFile(const char* path);
	virtual void smLog(const char* format,...);
	virtual void smScreenShot(const char* path);

	virtual void smSetFPS(int fps);
	virtual float smGetFPS();
	virtual float smGetDelta();
	virtual float smGetTime();

	virtual SMSFX smSFXLoad(const char *path);
	virtual SMSFX smSFXLoadFromMemory(const char *ptr,DWORD size);
	virtual SMCHN smSFXPlay(SMSFX fx,int vol=100,int pan=0,float pitch=1.,bool loop=0);
	virtual float smSFXGetLengthf(SMSFX fx);
	virtual DWORD smSFXGetLengthd(SMSFX fx);
	virtual void smSFXSetLoopPoint(SMSFX fx,DWORD l,DWORD r);
	virtual void smSFXFree(SMSFX fx);

	virtual void smChannelVol(SMCHN chn,int vol);
	virtual void smChannelPan(SMCHN chn,int pan);
	virtual void smChannelPitch(SMCHN chn,float pitch);
	virtual void smChannelPause(SMCHN chn);
	virtual void smChannelResume(SMCHN chn);
	virtual void smChannelStop(SMCHN chn);
	virtual void smChannelPauseAll();
	virtual void smChannelResumeAll();
	virtual void smChannelStopAll();
	virtual bool smChannelIsPlaying(SMCHN chn);
	virtual float smChannelGetPosf(SMCHN chn);
	virtual void smChannelSetPosf(SMCHN chn,float pos);
	virtual int smChannelGetPosd(SMCHN chn);
	virtual void smChannelSetPosd(SMCHN chn,int pos);

	virtual void smGetMouse2f(float *x,float *y);
	virtual void smSetMouse2f(float x,float y);
	virtual void smSetMouseGrab(bool enabled);
	virtual int smGetWheel();
	virtual bool smIsMouseOver();
	virtual int smGetKeyState(int key);
	virtual int smGetKey();
	virtual bool smGetInpEvent(smInpEvent *e);

	virtual bool smRenderBegin2D(bool ztest=0,SMTRG trg=0);
	virtual bool smRenderBegin3D(float fov,bool ztest=0,SMTRG trg=0);
	virtual bool smRenderEnd();
	virtual void sm3DCamera6f2v(float *pos,float *rot);
	virtual void sm2DCamera5f3v(float *pos,float *dpos,float *rot);
	virtual void smMultViewMatrix(float *mat);
	virtual void smClrscr(DWORD color,bool clearcol=true,bool cleardep=true);
	virtual void smRenderLinefd(float x1,float y1,float z1,float x2,float y2,float z2,DWORD color);
	virtual void smRenderLinefvd(float *p1,float *p2,DWORD color);
	virtual void smRenderTriangle(smTriangle *t);
	virtual void smRenderQuad(smQuad *q);
	virtual smVertex* smGetVertArray();
	virtual void smDrawVertArray(int prim,SMTEX texture,int blend,int _primcnt);
	virtual void smDrawCustomIndexedVertices(smVertex* vb,WORD* ib,int vbc,int ibc,int blend,SMTEX texture);

	virtual SMTRG smTargetCreate(int w,int h,int ms=0);
	virtual SMTEX smTargetTexture(SMTRG targ);
	virtual void smTargetFree(SMTRG targ);

	virtual SMTEX smTextureCreate(int w,int h);
	virtual SMTEX smTextureLoad(const char *path);
	virtual SMTEX smTextureLoadFromMemory(const char *ptr,DWORD size);
	virtual void smTextureFree(SMTEX tex);
	virtual void smTextureOpt(int potopt=TPOT_NONPOT,int filter=TFLT_LINEAR);
	virtual int smTextureGetWidth(SMTEX tex,bool original=false);
	virtual int smTextureGetHeight(SMTEX tex,bool original=false);
	virtual DWORD* smTextureLock(SMTEX tex,int l,int t,int w,int h,bool ro=true);
	virtual void smTexutreUnlock(SMTEX tex);
//internal routines&variables...
	static SMELT_IMPL* getInterface();
	void focusChange(bool actif);

	HINSTANCE hinstance;
	HWND *hwnd;
	bool Active;
	char curError[256];

	bool (*pUpdateFunc)();
	bool (*pUnFocFunc)();
	bool (*pFocFunc)();
	bool (*pQuitFunc)();
	smHandler *updateHandler,*unFocHandler,*focHandler,*quitHandler;
	const char *Icon;
	char winTitle[256];
	int scrw,scrh;
	int dispw,disph;
	bool windowed,vsync;
	char logFile[256];
	int limfps;
	bool hideMouse,noSuspend;

	D3DPRESENT_PARAMETERS* d3dpp;
	D3DPRESENT_PARAMETERS d3dppW;
	RECT rectW;
	LONG styleW;

	D3DPRESENT_PARAMETERS d3dppFS;
	RECT rectFS;
	LONG styleFS;
	IDirect3D9* pD3D;
	IDirect3DDevice9* pD3DDevice;
	IDirect3DVertexBuffer9* vertBuf;
	smVertex* vertArr;
	IDirect3DIndexBuffer9* indexBuf;
	IDirect3DSurface9* pScreenSurf;
	IDirect3DSurface9* pScreenDepth;
	TRenderTargetList *targets;
	TRenderTargetList *curTarget;
	TTextureList *textures;
	D3DXMATRIX matModView;
	D3DXMATRIX matProj;
	bool tdmode;
	int primcnt,primType,primBlend,filtermode;
	SMTEX primTex;
	bool zbufenabled;
	bool initD3D();
	void finiD3D();
	bool restD3D();
	bool confD3D();
	void batchD3D(bool endScene=false);
	void configTexture(glTexture *t,int w,int h,DWORD *px,bool compress=true);
	void bindTexture(glTexture *t);
	bool buildTarget(TRenderTargetList *pTarget,GLuint texid,int w,int h,int ms);
	SMTEX buildTexture(int w,int h,DWORD *px);
	void setBlend(int blend);
	void configProjectionMatrix2D(int w,int h);
	void configProjectionMatrix3D(int w,int h,float fov);
	DWORD* decodeImage(BYTE *data,const char *fn,DWORD size,int &w,int &h);

	void *pOpenALDevice;
	bool initOAL();
	void finiOAL();
	bool mute,lpp;
	int scnt;
	ALuint src[SRC_MAX];
	ALuint getSource();

	int posz,lkey;
	float posx,posy;
	bool mouseOver,captured;
	char keyz[256];
	bool keylst[256];
	TInputEventList *inpQueue;
	void initInput();
	void clearQueue();
	void buildEvent(int type,int k,int scan,int flags,int x,int y);
	bool procSDLEvent(const SDL_Event &e);
	int SDLKeyToSMKey(int sdlkey);

	float timeDelta,updateFPSDelay,fps,timeS;
	DWORD fixDelta,t0;
	int fcnt;
	DWORD dt;
private:
	SMELT_IMPL();
};
extern SMELT_IMPL *pSM;
#endif
