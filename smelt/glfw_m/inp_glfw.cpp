// -*- C++ -*-
/*
 * Simple MultimEdia LiTerator(SMELT)
 * by Chris Xiong 2015
 * Input manager implementation
 *
 * WARNING: This library is in development and interfaces would be very
 * unstable.
 *
 */
#include "smelt_internal.hpp"
//static const char* INP_GLFW_SRCFN="smelt/glfw/inp_glfw.cpp";
bool SMELT_IMPL::smGetInpEvent(smInpEvent *e)
{
	TInputEventList *eptr;
	if(inpQueue)
	{
		eptr=inpQueue;memcpy(e,&eptr->e,sizeof(smInpEvent));
		inpQueue=inpQueue->next;delete eptr;return true;
	}
	return false;
}
void SMELT_IMPL::smGetMouse2f(float *x,float *y){*x=posx,*y=posy;}
void SMELT_IMPL::smSetMouse2f(float x,float y){glfwSetInputMode((GLFWwindow*)hwnd,x,y);}
void SMELT_IMPL::smSetMouseGrab(bool enabled){glfwSetInputMode((GLFWwindow*)hwnd,GLFW_CURSOR,enabled?GLFW_CURSOR_DISABLED:GLFW_CURSOR_NORMAL);}
int SMELT_IMPL::smGetWheel(){return posz;}
bool SMELT_IMPL::smIsMouseOver(){return mouseOver;}
int SMELT_IMPL::smGetKey(){return lkey;}
int SMELT_IMPL::smGetKeyState(int key)
{
	if(!(keyz[key]&4)&&keylst[key])return SMKST_RELEASE;
	if(!(keyz[key]&4))return SMKST_NONE;
	if((keyz[key]&4)&&!keylst[key])return SMKST_HIT;
	return SMKST_KEEP;
}

void SMELT_IMPL::buildEvent(int type,int k,int scan,int flags,int x,int y)
{
	TInputEventList *last,*eptr=new TInputEventList;
	eptr->e.type=type;eptr->e.chcode=0;
	int ptx=x,pty=y;
	if(type==INPUT_KEYDOWN)
	{
		k=GLFWKeyToSMKey(k);
		if((k<0)||(k>(int)(sizeof(keyz)/sizeof(keyz[0]))))return;
		keyz[k]|=4;if(!(flags&SMINP_REPEAT))keyz[k]|=1;
		eptr->e.chcode=(k>=32&&k<=127)?k:0;
	}
	if(type==INPUT_KEYUP)
	{
		k=GLFWKeyToSMKey(k);
		if((k<0)||(k>(int)(sizeof(keyz)/sizeof(keyz[0]))))return;
		keyz[k]&=~4;keyz[k]|=2;
		eptr->e.chcode=(k>=32&&k<=127)?k:0;
	}
	if(type==INPUT_MOUSEWHEEL)
	{eptr->e.sccode=0;eptr->e.wheel=k;}
	else{eptr->e.sccode=k;eptr->e.wheel=0;}
	if(type==INPUT_MBUTTONDOWN){keyz[k]|=1|4;captured=true;}
	if(type==INPUT_MBUTTONUP){keyz[k]|=2;keyz[k]&=~4;ptx=(int)posx;pty=(int)posy;captured=false;}
	if(keymods&GLFW_MOD_SHIFT)flags|=SMINP_SHIFT;
	if(keymods&GLFW_MOD_CONTROL)flags|=SMINP_CTRL;
	if(keymods&GLFW_MOD_ALT)flags|=SMINP_ALT;
	//if(keymods&KMOD_CAPS)flags|=SMINP_CAPSLOCK;
	//if(keymods&KMOD_MODE)flags|=SMINP_SCROLLLOCK;
	//if(keymods&KMOD_NUM)flags|=SMINP_NUMLOCK;
	eptr->e.flag=flags;
	if(ptx==-1){eptr->e.x=posx;eptr->e.y=posy;}
	else
	{
		ptx<0?ptx=0:0;pty<0?pty=0:0;
		ptx>=scrw?ptx=scrw-1:0;pty>=scrh?pty=scrh-1:0;
		eptr->e.x=(float)ptx;eptr->e.y=(float)pty;
	}
	eptr->next=0;
	if(!inpQueue)inpQueue=eptr;
	else
	{
		last=inpQueue;
		while(last->next)last=last->next;
		last->next=eptr;
	}
	if(eptr->e.type==INPUT_KEYDOWN||eptr->e.type==INPUT_MBUTTONDOWN)
	lkey=eptr->e.sccode;
	if(eptr->e.type==INPUT_MOUSEMOVE)posx=eptr->e.x,posy=eptr->e.y;
	if(eptr->e.type==INPUT_MOUSEWHEEL)posz=eptr->e.wheel;
}
void SMELT_IMPL::initInput(){posx=posy=.0;memset(keyz,0,sizeof(keyz));}
void SMELT_IMPL::clearQueue()
{
	TInputEventList *nxt,*eptr=inpQueue;
	for(unsigned i=0;i<sizeof(keyz)/sizeof(keyz[0]);++i)keyz[i]&=~3;
	while(eptr){nxt=eptr->next;delete eptr;eptr=nxt;}
	inpQueue=NULL;posz=0;lkey=0;
}
int SMELT_IMPL::GLFWKeyToSMKey(int glfwkey)
{
	switch (glfwkey)
	{
		case GLFW_KEY_ESCAPE: return SMK_ESCAPE;
		case GLFW_KEY_BACKSPACE: return SMK_BACKSPACE;
		case GLFW_KEY_TAB: return SMK_TAB;
		case GLFW_KEY_ENTER: return SMK_ENTER;
		case GLFW_KEY_SPACE: return SMK_SPACE;
		case GLFW_KEY_LEFT_SHIFT: return SMK_SHIFT;
		case GLFW_KEY_RIGHT_SHIFT: return SMK_SHIFT;
		case GLFW_KEY_LEFT_CONTROL: return SMK_CTRL;
		case GLFW_KEY_RIGHT_CONTROL: return SMK_CTRL;
		case GLFW_KEY_LEFT_ALT: return SMK_ALT;
		case GLFW_KEY_RIGHT_ALT: return SMK_ALT;
		case GLFW_KEY_LEFT_SUPER: return SMK_LWIN;
		case GLFW_KEY_RIGHT_SUPER: return SMK_RWIN;
		case GLFW_KEY_PAUSE: return SMK_PAUSE;
		case GLFW_KEY_CAPS_LOCK: return SMK_CAPSLOCK;
		case GLFW_KEY_NUM_LOCK: return SMK_NUMLOCK;
		case GLFW_KEY_SCROLL_LOCK: return SMK_SCROLLLOCK;
		case GLFW_KEY_PAGE_UP: return SMK_PGUP;
		case GLFW_KEY_PAGE_DOWN: return SMK_PGDN;
		case GLFW_KEY_HOME: return SMK_HOME;
		case GLFW_KEY_END: return SMK_END;
		case GLFW_KEY_INSERT: return SMK_INSERT;
		case GLFW_KEY_DELETE: return SMK_DELETE;
		case GLFW_KEY_LEFT: return SMK_LEFT;
		case GLFW_KEY_UP: return SMK_UP;
		case GLFW_KEY_RIGHT: return SMK_RIGHT;
		case GLFW_KEY_DOWN: return SMK_DOWN;
		case GLFW_KEY_0: return SMK_0;
		case GLFW_KEY_1: return SMK_1;
		case GLFW_KEY_2: return SMK_2;
		case GLFW_KEY_3: return SMK_3;
		case GLFW_KEY_4: return SMK_4;
		case GLFW_KEY_5: return SMK_5;
		case GLFW_KEY_6: return SMK_6;
		case GLFW_KEY_7: return SMK_7;
		case GLFW_KEY_8: return SMK_8;
		case GLFW_KEY_9: return SMK_9;
		case GLFW_KEY_A: return SMK_A;
		case GLFW_KEY_B: return SMK_B;
		case GLFW_KEY_C: return SMK_C;
		case GLFW_KEY_D: return SMK_D;
		case GLFW_KEY_E: return SMK_E;
		case GLFW_KEY_F: return SMK_F;
		case GLFW_KEY_G: return SMK_G;
		case GLFW_KEY_H: return SMK_H;
		case GLFW_KEY_I: return SMK_I;
		case GLFW_KEY_J: return SMK_J;
		case GLFW_KEY_K: return SMK_K;
		case GLFW_KEY_L: return SMK_L;
		case GLFW_KEY_M: return SMK_M;
		case GLFW_KEY_N: return SMK_N;
		case GLFW_KEY_O: return SMK_O;
		case GLFW_KEY_P: return SMK_P;
		case GLFW_KEY_Q: return SMK_Q;
		case GLFW_KEY_R: return SMK_R;
		case GLFW_KEY_S: return SMK_S;
		case GLFW_KEY_T: return SMK_T;
		case GLFW_KEY_U: return SMK_U;
		case GLFW_KEY_V: return SMK_V;
		case GLFW_KEY_W: return SMK_W;
		case GLFW_KEY_X: return SMK_X;
		case GLFW_KEY_Y: return SMK_Y;
		case GLFW_KEY_Z: return SMK_Z;
		case GLFW_KEY_MINUS: return SMK_MINUS;
		case GLFW_KEY_EQUAL: return SMK_EQUALS;
		case GLFW_KEY_BACKSLASH: return SMK_BACKSLASH;
		case GLFW_KEY_LEFT_BRACKET: return SMK_LBRACKET;
		case GLFW_KEY_RIGHT_BRACKET: return SMK_RBRACKET;
		case GLFW_KEY_SEMICOLON: return SMK_SEMICOLON;
		case GLFW_KEY_APOSTROPHE: return SMK_APOSTROPHE;
		case GLFW_KEY_COMMA: return SMK_COMMA;
		case GLFW_KEY_PERIOD: return SMK_PERIOD;
		case GLFW_KEY_SLASH: return SMK_SLASH;
		case GLFW_KEY_KP_0: return SMK_NUMPAD0;
		case GLFW_KEY_KP_1: return SMK_NUMPAD1;
		case GLFW_KEY_KP_2: return SMK_NUMPAD2;
		case GLFW_KEY_KP_3: return SMK_NUMPAD3;
		case GLFW_KEY_KP_4: return SMK_NUMPAD4;
		case GLFW_KEY_KP_5: return SMK_NUMPAD5;
		case GLFW_KEY_KP_6: return SMK_NUMPAD6;
		case GLFW_KEY_KP_7: return SMK_NUMPAD7;
		case GLFW_KEY_KP_8: return SMK_NUMPAD8;
		case GLFW_KEY_KP_9: return SMK_NUMPAD9;
		case GLFW_KEY_KP_MULTIPLY: return SMK_MULTIPLY;
		case GLFW_KEY_KP_DIVIDE: return SMK_DIVIDE;
		case GLFW_KEY_KP_ADD: return SMK_ADD;
		case GLFW_KEY_KP_SUBTRACT: return SMK_SUBTRACT;
		case GLFW_KEY_KP_DECIMAL: return SMK_DECIMAL;
		case GLFW_KEY_KP_ENTER:return SMK_ENTER;
		case GLFW_KEY_F1: return SMK_F1;
		case GLFW_KEY_F2: return SMK_F2;
		case GLFW_KEY_F3: return SMK_F3;
		case GLFW_KEY_F4: return SMK_F4;
		case GLFW_KEY_F5: return SMK_F5;
		case GLFW_KEY_F6: return SMK_F6;
		case GLFW_KEY_F7: return SMK_F7;
		case GLFW_KEY_F8: return SMK_F8;
		case GLFW_KEY_F9: return SMK_F9;
		case GLFW_KEY_F10: return SMK_F10;
		case GLFW_KEY_F11: return SMK_F11;
		case GLFW_KEY_F12: return SMK_F12;
		default: return -1;
	}
	return -1;
}
