// -*- C++ -*-
/*
 * Simple MultimEdia LiTerator(SMELT)
 * by Chris Xiong 2015
 * Internal header for GLFW version
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
#include <string>
#ifdef WIN32
#include <windows.h>
#include <intrin.h>
#define strcasecmp _stricmp
#else
#include <dirent.h>
#include <unistd.h>
#endif
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifndef ENABLE_DUMB
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#ifdef ENABLE_OGG_SUPPORT
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>
#endif
#endif

#define xstr(s) str(s)
#define str(s) #s
#define SLINE xstr(__LINE__)

class TOpenGLDevice
{
public:
	bool have_base_opengl;
#ifdef USE_OPENGL_COMPATIBILITY_PROFILE
	GLenum TextureTarget;
	bool have_GL_ARB_texture_rectangle;
	bool have_GL_ARB_texture_non_power_of_two;
	bool have_GL_EXT_framebuffer_object;
	bool have_GL_EXT_texture_compression_s3tc;
	bool have_GL_ARB_vertex_buffer_object;
	bool have_GL_EXT_framebuffer_multisample;
	bool have_GL_EXT_framebuffer_blit;
#endif
};

struct glTexture;
class TRenderTargetList
{
public:
	int w,h,ms;
	SMTEX tex;
	GLuint depth,frame,colorms,sframe,sdepth,scolor;
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
	virtual void smVidMode(int resX, int resY, bool _windowed, bool _showWindow=true);
	virtual void smLogFile(const char* path);
	virtual void smLog(const char* format,...);
	virtual void smScreenShot(const char* path);
	virtual void smPixelCopy(int origx,int origy,size_t w,size_t h,size_t sz,void* px);

	virtual void smSetFPS(int fps);
	virtual float smGetFPS();
	virtual float smGetDelta();
	virtual float smGetTime();

	virtual const char* smGetCPUModel();
	virtual const char* smGetOSInfo();
	virtual const char* smGetDispDriver();

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

	static SMELT_IMPL* getInterface();
private:
	void focusChange(bool actif);

	void *hwnd;
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
	bool windowed,vsync,showwindow;
	char logFile[256];
	int limfps;
	bool hideMouse,noSuspend;
	std::string osver,cpumodel,dispdrv;

	TOpenGLDevice *pOpenGLDevice;
	smVertex *vertexArray;
	smVertex *vertexBuf;
	GLushort *indexBuf;
	GLuint IndexBufferObject;
#ifndef USE_OPENGL_COMPATIBILITY_PROFILE
	GLuint VertexBufferObject;
	GLuint VertexArrayObject;
	GLuint ShaderProgram;
	GLuint fragshader,vertshader;
	int loc_tex,loc_mmodv,loc_mproj;
	float mmodv[16],mproj[16];
	SMTEX emptyTex;
#endif
	TRenderTargetList *targets;
	TRenderTargetList *curTarget;
	TTextureList *textures;
	bool tdmode;
	int primcnt,primType,primBlend,filtermode;
	SMTEX primTex;
	bool zbufenabled;
	bool checkGLExtension(const char *extlist,const char *ext);
	void unloadGLEntryPoints();
	bool loadGLEntryPoints();
	bool initOGL();
	void finiOGL();
	bool restOGL();
	bool confOGL();
	void batchOGL(bool endScene=false);
	void configTexture(glTexture *t,int w,int h,DWORD *px,bool compress=true);
	void bindTexture(glTexture *t);
	bool buildTarget(TRenderTargetList *pTarget,GLuint texid,int w,int h,int ms);
	SMTEX buildTexture(int w,int h,DWORD *px);
	void setBlend(int blend);
	void configProjectionMatrix2D(int w,int h);
	void configProjectionMatrix3D(int w,int h,float fov);
	DWORD* decodeImage(BYTE *data,const char *fn,DWORD size,int &w,int &h);

	void *pOpenALDevice;
	bool mute,lpp;
	bool initOAL();
	void finiOAL();
#ifndef ENABLE_DUMB
	int scnt;
	ALuint src[SRC_MAX];
	ALuint getSource();
#endif

	int posz,lkey;
	float posx,posy;
	int keymods;
	bool mouseOver,captured;
	char keyz[256];
	bool keylst[256];
	TInputEventList *inpQueue;
	void initInput();
	void clearQueue();
	void buildEvent(int type,int k,int scan,int flags,int x,int y);
	bool procGLFWEvent();
	int GLFWKeyToSMKey(int glfwkey);

	float timeDelta,updateFPSDelay,fps,timeS;
	DWORD fixDelta,t0;
	int fcnt;
	DWORD dt;
	SMELT_IMPL();

	friend class glfwHandlers;
};
extern SMELT_IMPL *pSM;
#endif
