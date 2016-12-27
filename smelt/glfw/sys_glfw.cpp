// -*- C++ -*-
/*
 * Simple MultimEdia LiTerator(SMELT)
 * by Chris Xiong 2015
 * SMELT system implementation
 *
 * WARNING: This library is in development and interfaces would be very
 * unstable.
 *
 */
#include "smelt_internal.hpp"
#include <thread>
#include <chrono>
static const char* SYS_GLFW_SRCFN="smelt/glfw/sys_glfw.cpp";
int refcnt=0;
SMELT_IMPL *pSM=0;
char lasterr[1024];

void glfwErrorHandler(int err,const char* desc);
void glfwFocChangeHandler(GLFWwindow*,int foc);
void glfwCursorEnterHandler(GLFWwindow*,int ent);
void glfwKBEventHandler(GLFWwindow* window,int key,int scancode,int action,int mods);
void glfwMouseButtonHandler(GLFWwindow* w,int btn,int action,int mods);
void glfwMouseMotionHandler(GLFWwindow*,double x,double y);
void glfwMouseWheelHandler(GLFWwindow* w,double x,double y);

SMELT* smGetInterface(int apilevel)
{
	if(apilevel==SMELT_APILEVEL)
	return(SMELT*)SMELT_IMPL::getInterface();
	else return 0;
}

SMELT_IMPL* SMELT_IMPL::getInterface()
{
	if(!pSM)pSM=new SMELT_IMPL();
	++refcnt;return pSM;
}

void SMELT_IMPL::smRelease()
{
	--refcnt;
	if(!refcnt)
	{
		if(pSM->hwnd)pSM->smFinale();
		delete pSM;pSM=0;
	}
}

bool SMELT_IMPL::smInit()
{
	smLog("%s:" SLINE ": Initalizing SMELT...\n",SYS_GLFW_SRCFN);
	smLog("%s:" SLINE ": SMELT api version %d\n",SYS_GLFW_SRCFN,SMELT_APILEVEL);
	time_t t=time(NULL);
	smLog("%s:" SLINE ": Date %s",SYS_GLFW_SRCFN,asctime(localtime(&t)));
#ifdef WIN32
	OSVERSIONINFO os_ver; MEMORYSTATUS mem_st;
	os_ver.dwOSVersionInfoSize=sizeof(os_ver);
	GetVersionEx(&os_ver);
	smLog("%s:" SLINE ": OS: Windows %ld.%ld.%ld\n", SYS_GLFW_SRCFN,os_ver.dwMajorVersion,os_ver.dwMinorVersion,os_ver.dwBuildNumber);

	int CPUInfo[4]={-1};
	__cpuid(CPUInfo,0x80000000);
	unsigned int nExIds=CPUInfo[0];
	char *cpuName,*loced;cpuName=calloc(0x40,sizeof(char));loced=cpuName;
	for(unsigned int i=0x80000000;i<=nExIds;++i)
	{
		__cpuid(CPUInfo, i);
		if(i==0x80000002)
			memcpy(cpuName,CPUInfo,sizeof(CPUInfo));
		else if(i==0x80000003)
			memcpy(cpuName+16,CPUInfo,sizeof(CPUInfo));
		else if(i==0x80000004)
		memcpy(cpuName+32, CPUInfo, sizeof(CPUInfo));
	}
	while(*cpuName==' ')++cpuName;
	smLog("%s:" SLINE ": CPU: %s\n", SYS_GLFW_SRCFN,cpuName);
	free(loced);

	GlobalMemoryStatus(&mem_st);
	smLog("%s:" SLINE ": Memory: %ldK total, %ldK free\n", SYS_GLFW_SRCFN,mem_st.dwTotalPhys/1024L,mem_st.dwAvailPhys/1024L);
#else
	system("uname -svm > /tmp/os.out");
	char osv[100];FILE* a=fopen("/tmp/os.out","r");fgets(osv,100,a);fclose(a);
	osv[strlen(osv)-1]='\0';
	smLog("%s:" SLINE ": OS: %s\n",SYS_GLFW_SRCFN,osv);
	system("rm /tmp/os.out");

	system("cat /proc/cpuinfo | grep name -m 1 > /tmp/cpu.out");
	a=fopen("/tmp/cpu.out","r");fgets(osv,100,a);fclose(a);
	osv[strlen(osv)-1]='\0';char *ptr=osv;while(*ptr!=':')++ptr;ptr+=2;
	smLog("%s:" SLINE ": CPU: %s\n",SYS_GLFW_SRCFN,osv);
	system("rm /tmp/cpu.out");

	a=fopen("/proc/meminfo","r");
	unsigned totalm,freem;
	fscanf(a,"MemTotal: %u kB\n",&totalm);
	fscanf(a,"MemFree: %u kB\n",&freem);
	smLog("%s:" SLINE ": RAM: %ukB installed, %ukB free\n",SYS_GLFW_SRCFN,totalm,freem);
	fclose(a);
#endif
	glfwSetErrorCallback(glfwErrorHandler);
	if(!glfwInit())
	{
		smLog("%s:" SLINE ": glfwInit() failed with error %s\n",SYS_GLFW_SRCFN,lasterr);
		return false;
	}
	GLFWmonitor *moninfo=glfwGetPrimaryMonitor();
	glfwGetMonitorPhysicalSize(moninfo,&dispw,&disph);
	smLog("%s:" SLINE ": Screen: %d x %d\n",SYS_GLFW_SRCFN,dispw,disph);
	glfwWindowHint(GLFW_RED_BITS,8);
	glfwWindowHint(GLFW_GREEN_BITS,8);
	glfwWindowHint(GLFW_BLUE_BITS,8);
	glfwWindowHint(GLFW_ALPHA_BITS,8);
	glfwWindowHint(GLFW_DEPTH_BITS,16);
	glfwWindowHint(GLFW_DOUBLEBUFFER,1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
	GLFWwindow *screen=glfwCreateWindow(windowed?scrw:dispw,windowed?scrh:disph,winTitle,NULL,NULL);
	hwnd=(void*)screen;
	if(!hwnd)
	{
		smLog("%s:" SLINE ": glfwCreateWindow() failed with error %s\n",SYS_GLFW_SRCFN,lasterr);
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(screen);
	glfwSwapInterval(vsync?1:0);
	if(!windowed)
	{
		mouseOver=true;
		if(!pSM->Active)pSM->focusChange(true);
	}
	if(hideMouse)glfwSetInputMode(screen,GLFW_CURSOR,GLFW_CURSOR_HIDDEN);
	glfwSetWindowFocusCallback(screen,glfwFocChangeHandler);
	glfwSetCursorEnterCallback(screen,glfwCursorEnterHandler);
	glfwSetKeyCallback(screen,glfwKBEventHandler);
	glfwSetCursorPosCallback(screen,glfwMouseMotionHandler);
	glfwSetMouseButtonCallback(screen,glfwMouseButtonHandler);
	glfwSetScrollCallback(screen,glfwMouseWheelHandler);
	initInput();
	if(!initOGL()){smFinale();return false;}
	if(!initOAL()){smFinale();return false;}
	t0=DWORD(glfwGetTime()*1000.);dt=fcnt=0;fps=.0;
	updateFPSDelay=.0;
	return true;
}

void SMELT_IMPL::smFinale()
{
	smLog("%s:" SLINE ": Cleaning up...\n",SYS_GLFW_SRCFN);
	clearQueue();finiOAL();finiOGL();
	glfwDestroyWindow((GLFWwindow*)hwnd);glfwTerminate();hwnd=0;
}

void SMELT_IMPL::smMainLoop()
{
	if(!hwnd)return smLog("%s:" SLINE ": Error: SMELT is not initialized.\n",SYS_GLFW_SRCFN);
	if(!pUpdateFunc&&!updateHandler) return smLog("%s:" SLINE ": UpdateFunc is not defined.\n",SYS_GLFW_SRCFN);
	Active=true;
	for(;;)
	{
		glfwPollEvents();
		bool loopcont=true;
		loopcont=procGLFWEvent();
		if(!loopcont)break;
		if(Active||noSuspend)
		{
			DWORD ticks;
			do{ticks=DWORD(glfwGetTime()*1000.);dt=ticks-t0;}while(dt<1);
			if(dt>=fixDelta)
			{
				timeDelta=dt/1000.;
				if(timeDelta>0.2)timeDelta=fixDelta?fixDelta/1000.:.01;
				++fcnt;updateFPSDelay+=timeDelta;t0=ticks;timeS+=timeDelta;
				if(updateFPSDelay>1){fps=fcnt/updateFPSDelay;updateFPSDelay=.0;fcnt=0;}
				if(pUpdateFunc){if(pUpdateFunc())break;}
				if(updateHandler){if(updateHandler->handlerFunc())break;}
				for(int i=1;i<=255;++i)keylst[i]=((keyz[i]&4)!=0);
				clearQueue();
			}
			else
			if(fixDelta&&dt+3<fixDelta)std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}else std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	clearQueue();Active=false;
}
void SMELT_IMPL::smUpdateFunc(smHook func){pUpdateFunc=func;}
void SMELT_IMPL::smUpdateFunc(smHandler* h){updateHandler=h;}
void SMELT_IMPL::smUnFocFunc(smHook func){pUnFocFunc=func;}
void SMELT_IMPL::smUnFocFunc(smHandler* h){unFocHandler=h;}
void SMELT_IMPL::smFocFunc(smHook func){pFocFunc=func;}
void SMELT_IMPL::smFocFunc(smHandler* h){focHandler=h;}
void SMELT_IMPL::smQuitFunc(smHook func){pQuitFunc=func;}
void SMELT_IMPL::smQuitFunc(smHandler* h){quitHandler=h;}
void SMELT_IMPL::smWinTitle(const char *title){strcpy(winTitle,title);}
bool SMELT_IMPL::smIsActive(){return Active;}
void SMELT_IMPL::smNoSuspend(bool para){noSuspend=para;}
void SMELT_IMPL::smVidMode(int resX,int resY,bool _windowed)
{
	if(vertexArray)return;
	if(!pOpenGLDevice)scrw=resX,scrh=resY,windowed=_windowed;
	else if(windowed!=_windowed)
	{
		windowed=_windowed;
		glfwSetWindowSize((GLFWwindow*)hwnd,windowed?scrw:dispw,windowed?scrh:disph);
		restOGL();
		if(!windowed){mouseOver=true;if(!pSM->Active)pSM->focusChange(true);}
	}
}
void SMELT_IMPL::smLogFile(const char* path)
{
	strcpy(logFile,path);
	FILE *tf=fopen(logFile,"w");
	if(!tf)*logFile=0;else fclose(tf);
}
void SMELT_IMPL::smLog(const char* format,...)
{
	va_list ap;
	va_start(ap,format);vfprintf(stderr,format,ap);va_end(ap);
	FILE *logf=fopen(logFile,"a");if(!logf)return;
	va_start(ap,format);vfprintf(logf,format,ap);va_end(ap);
	fclose(logf);
}
void SMELT_IMPL::smScreenShot(const char* path)
{
	if(pOpenGLDevice)
	{
		//glFinish();
		//glReadPixels(0,0,screen->w,screen->h,GL_RGB,GL_UNSIGNED_BYTE,surface->pixels);
	}
}
void SMELT_IMPL::smSetFPS(int fps)
{
	vsync=(fps==FPS_VSYNC);
	if(pOpenGLDevice)glfwSwapInterval(vsync?1:0);
	if(fps>0)fixDelta=1000./fps;else fixDelta=0;
}
float SMELT_IMPL::smGetFPS(){return fps;}
float SMELT_IMPL::smGetDelta(){return timeDelta;}
float SMELT_IMPL::smGetTime(){return timeS;}

SMELT_IMPL::SMELT_IMPL()
{
	hwnd=NULL;Active=false;memset(curError,0,sizeof(curError));
	pUpdateFunc=pUnFocFunc=pFocFunc=pQuitFunc=NULL;
	updateHandler=unFocHandler=focHandler=quitHandler=NULL;
	Icon=NULL;strcpy(winTitle,"untitled");scrw=dispw=800;scrh=disph=600;
	windowed=vsync=false;memset(logFile,0,sizeof(logFile));
	limfps=0;hideMouse=true;noSuspend=false;
	pOpenGLDevice=NULL;targets=NULL;curTarget=NULL;
	textures=NULL;vertexArray=NULL;vertexBuf=NULL;indexBuf=NULL;primTex=0;
	pOpenALDevice=NULL;mute=false;inpQueue=NULL;posz=0;posx=posy=.0;
	mouseOver=true;captured=false;keymods=0;zbufenabled=false;
	timeDelta=updateFPSDelay=fps=.0;fcnt=dt=fixDelta=0;
}
void SMELT_IMPL::focusChange(bool actif)
{
	Active=actif;
	if(actif){pFocFunc?pFocFunc():0;focHandler?focHandler->handlerFunc():0;}
	else {pUnFocFunc?pUnFocFunc():0;unFocHandler?unFocHandler->handlerFunc():0;};
}
bool SMELT_IMPL::procGLFWEvent()
{
	if(glfwWindowShouldClose((GLFWwindow*)hwnd))
	{
		bool accepted=true;
		if(pSM->pQuitFunc&&pSM->pQuitFunc())accepted=false;
		if(pSM->quitHandler&&quitHandler->handlerFunc())accepted=false;
		if(accepted)return false;
	}
	return true;
}
void glfwErrorHandler(int err,const char* desc)
{
	strcpy(lasterr,desc);
}
void glfwFocChangeHandler(GLFWwindow*,int foc)
{
	pSM->focusChange(foc);
}
void glfwCursorEnterHandler(GLFWwindow*,int ent)
{
	pSM->mouseOver=ent;
}
void glfwKBEventHandler(GLFWwindow* window,int key,int scancode,int action,int mods)
{
	if(action==GLFW_RELEASE)
	{
		pSM->keymods=mods;
		pSM->buildEvent(INPUT_KEYUP,key,0,0,-1,-1);
	}
	else
	{
		pSM->keymods=mods;
		if((pSM->keymods&GLFW_MOD_ALT)&&((key==GLFW_KEY_ENTER)||(key==GLFW_KEY_KP_ENTER)))
		pSM->smVidMode(pSM->scrw,pSM->scrh,!pSM->windowed);
		pSM->buildEvent(INPUT_KEYDOWN,key,0,0,-1,-1);
	}
}
void glfwMouseButtonHandler(GLFWwindow* w,int btn,int action,int mods)
{
	double x,y;glfwGetCursorPos(w,&x,&y);
	if(action==GLFW_PRESS)
	{
		if(btn==GLFW_MOUSE_BUTTON_LEFT)
		pSM->buildEvent(INPUT_MBUTTONDOWN,SMK_LBUTTON,0,0,x,y);
		if(btn==GLFW_MOUSE_BUTTON_RIGHT)
		pSM->buildEvent(INPUT_MBUTTONDOWN,SMK_RBUTTON,0,0,x,y);
		if(btn==GLFW_MOUSE_BUTTON_MIDDLE)
		pSM->buildEvent(INPUT_MBUTTONDOWN,SMK_MBUTTON,0,0,x,y);
	}
	else
	{
		if(btn==GLFW_MOUSE_BUTTON_LEFT)
		pSM->buildEvent(INPUT_MBUTTONUP,SMK_LBUTTON,0,0,x,y);
		if(btn==GLFW_MOUSE_BUTTON_RIGHT)
		pSM->buildEvent(INPUT_MBUTTONUP,SMK_RBUTTON,0,0,x,y);
		if(btn==GLFW_MOUSE_BUTTON_MIDDLE)
		pSM->buildEvent(INPUT_MBUTTONUP,SMK_MBUTTON,0,0,x,y);
	}
}
void glfwMouseMotionHandler(GLFWwindow*,double x,double y)
{
	pSM->buildEvent(INPUT_MOUSEMOVE,0,0,0,x,y);
}
void glfwMouseWheelHandler(GLFWwindow* w,double x,double y)
{
	double cx,cy;glfwGetCursorPos(w,&cx,&cy);
	if(y>0)pSM->buildEvent(INPUT_MOUSEWHEEL,1,0,0,cx,cy);
	if(y<0)pSM->buildEvent(INPUT_MOUSEWHEEL,-1,0,0,cx,cy);
}
