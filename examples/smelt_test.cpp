/*
 * The smelt testing/example program.
 * Resources required:
 * SpriteSheet.png lcdfont.anm
 * tap.ogg
 * 稲田姫樣に叱られるから.ogg(not included, just the original track in TH10)
 * A truetype font is also required.
 *
 * Expected result:
 * "bitmap font test" on the top in a 5*8 LCD font.
 * A fixed display of SpriteSheet.png.
 * A 3D-rotating SpriteSheet.png.
 * A rotating part of SpriteSheet.png with the
 * vertices color changed.
 * A circular indicator with its value changing.
 * Two lines of text in a truetype font.
 */
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
//This function handles the SIGSEGV signal, just ignore it.
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
//This function is called every frame.
{
	//we play the tapping sound every 0.37 seconds.
	//do not ask why 0.37 seconds.
	d+=sm->smGetDelta();
	if(d>0.37){sm->smSFXPlay(sfx);d=0;}

	//We render the 3D scene to a render target.
	sm->smRenderBegin3D(60,testtrg);
	//camera position and rotation.
	static float pos[3]={0,0,600};
	static float rot[3]={0,0,-30};
	//set the camera transformation.
	sm->sm3DCamera6f2v(pos,rot);
	//clear the rendering target
	sm->smClrscr(0xFF000000);
	//render the 3d entity.
	e3d->render9f(0,0,0,theta,0,1,0,1);
#define pp printf("(%.0f,%.0f,%.0f) (%.0f,%.0f,%.0f)\n",pos[0],pos[1],pos[2],rot[0],rot[1],rot[2]);
	//camera manipulation key bindings.
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
	//set the rendering target to the screen.
	sm->smRenderEnd();
	sm->smRenderBegin2D();
	//clear the screen.
	sm->smClrscr(0xFF000000);
	//render a test string with the bitmap font.
	fonttest.render(0,0,0,ALIGN_LEFT,NULL,"bitmap font test");
	//render a test string with the truetype font.
	ttftest.render(0,500,0xFFFFFFFF,ALIGN_LEFT);
	//render the content of the previously used rendering
	//target to the screen.
	quad.tex=sm->smTargetTexture(testtrg);
	sm->smRenderQuad(&quad);
	//render the testing 2d entity.
	e2d->render(400,300,theta);
	double t;
	//set the value of the circular indicator and render it.
	ictest.setValue(modf(theta,&t));
	ictest.render(400,300);
	theta+=0.02;
	//render a testing quad.
	quad.tex=tex;
	sm->smRenderQuad(&quad);
	sm->smRenderEnd();
	//screenshot test.
	if(sm->smGetKeyState(SMK_Y)==SMKST_HIT)sm->smScreenShot("wtf.bmp");
	return false;
}
int main()
{
	//set the signal handler.
	signal(SIGSEGV,sigHandler);
	//get the pointer to the smelt core.
	sm=smGetInterface(SMELT_APILEVEL);
	//set the video mode.
	sm->smVidMode(800,600,true);
	//set the update function.
	sm->smUpdateFunc(doingNothing);
	//set the window title.
	sm->smWinTitle("SMELT Example App");
	//set the FPS policy.
	sm->smSetFPS(FPS_VSYNC);
	//now we can initialize the smelt engine.
	sm->smInit();

	//As anm files are always packed in dtp files, only the
	//load-from-memory version is implemented.
	//We load the bitmap font here.
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

	//load the truetype font.
	if(!ttftest.loadTTF("/usr/share/fonts/truetype/wqy/wqy-microhei.ttc",18))sm->smLog("ttf load error!");

	//load the texture.
	tex=sm->smTextureLoad("SpriteSheet.png");
	//load the sound files.
	sfx=sm->smSFXLoad("tap.ogg");
	bgm=sm->smSFXLoad("稲田姫樣に叱られるから.ogg");
	//set loop points of the bgm.
	sm->smSFXSetLoopPoint(bgm,0xED80,0x1E0400);
	//create the rendering target for the 3d scene.
	testtrg=sm->smTargetCreate(256,320);
	//create the entities.
	e2d=new smEntity2D(tex,0,0,101,101);
	e2d->setCentre(50,50);
	e2d->setColor(0xFFFFFFFF,0);
	e2d->setColor(0xFFFF0000,1);
	e2d->setColor(0xFF00FF00,2);
	e2d->setColor(0xFF0000FF,3);
	e3d=new smEntity3D(tex,0,0,256,320);
	e3d->setCentre(128,160);
	//set up the quad.
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
	//initialize the indicator.
	ictest.init(100,5,0x80,tex,smTexRect(151,264,2,8));
	//set the string of the truetype font renderer.
	ttftest.updateString(L"truetype font test\nNow Playing: 稲田姫樣に叱られるから by ZUN");
	//play the bgm.
	sm->smSFXPlay(bgm,100,0,1.,true);
	//enter the main loop.
	sm->smMainLoop();
	//deinitialize the smelt engine
	sm->smFinale();
	//free the resources we used.
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
	//destroy the smelt interface.
	sm->smRelease();
	return 0;
}
