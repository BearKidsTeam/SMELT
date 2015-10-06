#include "smelt.hpp"
#include "smentity.hpp"
#include "smindicator.hpp"
#include "smbmfont.hpp"
#include "smttfont.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#if defined(__GNUC__) && !defined(MINGW_BUILD)
#include <execinfo.h>
#endif
SMELT *sm;
smEntity2D *e2d;
smEntity3D *e3d;
smQuad quad;
float theta,d;
indicatorCircular ictest;
SMTEX tex;
SMTRG testtrg;
smBMFont fonttest;
smTTFont ttftest;
SMSFX sfx,bgm;
void sigHandler(int pm)
{
	sm->smLog("Oops, the application ate a piece of DEAD BEEF!\n");
#if defined(__GNUC__) && !defined(MINGW_BUILD)
	void *strs[64];unsigned cnt;
	char **str;cnt=backtrace(strs,64);
	str=backtrace_symbols(strs,cnt);
	for(unsigned i=0;i<cnt;++i)
		sm->smLog("%s\n",str[i]);
#endif
	sm->smFinale();
	exit(1);
}
bool doingNothing()
{
	d+=sm->smGetDelta();
	if(d>0.37){sm->smSFXPlay(sfx);d=0;}
	sm->smRenderBegin3D(60,testtrg);
	static float pos[3]={0,0,600};
	static float rot[3]={0,0,-30};
	sm->sm3DCamera6f2v(pos,rot);
	sm->smClrscr(0xFF000000);
	e3d->render9f(0,0,0,theta,0,1,0,1);
	//printf("%.2f\n",sm->smGetFPS());
#define pp printf("(%.0f,%.0f,%.0f) (%.0f,%.0f,%.0f)\n",pos[0],pos[1],pos[2],rot[0],rot[1],rot[2]);
	if(sm->smGetKeyState(SMK_W)==SMKST_HIT)pos[1]+=10,pp;
	if(sm->smGetKeyState(SMK_S)==SMKST_HIT)pos[1]-=10,pp;
	if(sm->smGetKeyState(SMK_D)==SMKST_HIT)pos[0]+=10,pp;
	if(sm->smGetKeyState(SMK_A)==SMKST_HIT)pos[0]-=10,pp;
	if(sm->smGetKeyState(SMK_Q)==SMKST_HIT)pos[2]+=10,pp;
	if(sm->smGetKeyState(SMK_E)==SMKST_HIT)pos[2]-=10,pp;
	if(sm->smGetKeyState(SMK_I)==SMKST_HIT)rot[1]+=5,pp;
	if(sm->smGetKeyState(SMK_K)==SMKST_HIT)rot[1]-=5,pp;
	if(sm->smGetKeyState(SMK_L)==SMKST_HIT)rot[0]+=5,pp;
	if(sm->smGetKeyState(SMK_J)==SMKST_HIT)rot[0]-=5,pp;
	if(sm->smGetKeyState(SMK_U)==SMKST_HIT)rot[2]+=5,pp;
	if(sm->smGetKeyState(SMK_O)==SMKST_HIT)rot[2]-=5,pp;
	sm->smRenderEnd();
	sm->smRenderBegin2D();
	sm->smClrscr(0xFF000000);
	fonttest.render(0,0,0,ALIGN_LEFT,NULL,"bitmap font test");
	ttftest.render(0,500,0xFFFFFFFF,ALIGN_LEFT);
	quad.tex=sm->smTargetTexture(testtrg);
	sm->smRenderQuad(&quad);
	e2d->render(400,300,theta);
	double t;
	ictest.setValue(modf(theta,&t));
	ictest.render(400,300);
	theta+=0.02;
	quad.tex=tex;
	sm->smRenderQuad(&quad);
	sm->smRenderEnd();
	if(sm->smGetKeyState(SMK_Y)==SMKST_HIT)sm->smScreenShot("wtf.bmp");
	return false;
}
int main()
{
	signal(SIGSEGV,sigHandler);
	sm=smGetInterface(SMELT_APILEVEL);
	sm->smVidMode(800,600,true);
	sm->smUpdateFunc(doingNothing);
	sm->smWinTitle("SMELT Example App");
	sm->smSetFPS(FPS_VSYNC);
	sm->smInit();

	FILE *pFile;DWORD size,rsize;char *buff;
	pFile=fopen("lcdfont.anm","rb");
	if(!pFile)return 0;
	fseek(pFile,0,SEEK_END);size=ftell(pFile);rewind(pFile);
	buff=(char*)malloc(sizeof(char)*size);
	if(!buff)return 0;
	rsize=fread(buff,1,size,pFile);
	if(rsize!=size)return 0;
	fonttest.loadAnmFromMemory(buff,size);
	fonttest.setColor(0xFFFFFFFF);
	fonttest.setBlend(BLEND_ALPHABLEND);
	fonttest.setScale(1.);

	if(!ttftest.loadTTF("/usr/share/fonts/truetype/wqy/wqy-microhei.ttc",18))sm->smLog("ttf load error!");

	tex=sm->smTextureLoad("SpriteSheet.png");
	sfx=sm->smSFXLoad("tap.ogg");
	bgm=sm->smSFXLoad("稲田姫樣に叱られるから.ogg");
	sm->smSFXSetLoopPoint(bgm,0xED80,0x1E0400);
	//sm->smTextureLock(tex,0,0,64,320,false);
	//sm->smTexutreUnlock(tex);
	testtrg=sm->smTargetCreate(256,320);
	e2d=new smEntity2D(tex,0,0,101,101);
	e2d->setCentre(50,50);
	e2d->setColor(0xFFFFFFFF,0);
	e2d->setColor(0xFFFF0000,1);
	e2d->setColor(0xFF00FF00,2);
	e2d->setColor(0xFF0000FF,3);
	e3d=new smEntity3D(tex,0,0,256,320);
	e3d->setCentre(128,160);
	quad.v[0].x=128;quad.v[0].y=128;
	quad.v[1].x=384;quad.v[1].y=128;
	quad.v[2].x=384;quad.v[2].y=448;
	quad.v[3].x=128;quad.v[3].y=448;
	quad.v[0].tx=.0;quad.v[0].ty=.0;
	quad.v[1].tx=1.;quad.v[1].ty=.0;
	quad.v[2].tx=1.;quad.v[2].ty=1.;
	quad.v[3].tx=.0;quad.v[3].ty=1.;
	for(int i=0;i<4;++i)quad.v[i].col=0xFFFFFFFF;
	quad.blend=BLEND_ALPHABLEND;
	quad.tex=tex;
	ictest.init(100,5,0x80,tex,smTexRect(151,264,2,8));
	ttftest.updateString(L"truetype font test\nNow Playing: 稲田姫樣に叱られるから by ZUN");
	sm->smSFXPlay(bgm,100,0,1.,true);
	sm->smMainLoop();
	sm->smFinale();
	delete e2d;
	ictest.deinit();
	fonttest.close();
	free(buff);
	fclose(pFile);
	ttftest.releaseTTF();
	sm->smTextureFree(tex);
	sm->smTargetFree(testtrg);
	sm->smSFXFree(sfx);
	sm->smSFXFree(bgm);
	sm->smRelease();
	return 0;
}
