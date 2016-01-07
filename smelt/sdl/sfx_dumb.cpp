// -*- C++ -*-
/*
 * Simple MultimEdia LiTerator(SMELT)
 * by Chris Xiong 2015
 * SFX dumb implementation
 * This dumb implementation has everything stubbed, useful if you don't
 * use the audio routines here.
 *
 * WARNING: This library is in development and interfaces would be very
 * unstable.
 *
 */
#include "smelt_internal.hpp"

static const char* SFX_SDL_SRCFN="smelt/sdl/sfx_dumb.cpp";

SMSFX SMELT_IMPL::smSFXLoad(const char *path)
{return 0;}
SMSFX SMELT_IMPL::smSFXLoadFromMemory(const char *ptr,DWORD size)
{return 0;}
SMCHN SMELT_IMPL::smSFXPlay(SMSFX fx,int vol,int pan,float pitch,bool loop)
{return 0;}
float SMELT_IMPL::smSFXGetLengthf(SMSFX fx)
{return 0.0;}
DWORD SMELT_IMPL::smSFXGetLengthd(SMSFX fx)
{return -1;}
void SMELT_IMPL::smSFXSetLoopPoint(SMSFX fx,DWORD l,DWORD r)
{}
void SMELT_IMPL::smSFXFree(SMSFX fx)
{}
void SMELT_IMPL::smChannelVol(SMCHN chn,int vol)
{}
void SMELT_IMPL::smChannelPan(SMCHN chn,int pan)
{}
void SMELT_IMPL::smChannelPitch(SMCHN chn,float pitch)
{}
void SMELT_IMPL::smChannelPause(SMCHN chn)
{}
void SMELT_IMPL::smChannelResume(SMCHN chn)
{}
void SMELT_IMPL::smChannelStop(SMCHN chn)
{}
void SMELT_IMPL::smChannelPauseAll()
{}
void SMELT_IMPL::smChannelResumeAll()
{}
void SMELT_IMPL::smChannelStopAll()
{}
bool SMELT_IMPL::smChannelIsPlaying(SMCHN chn)
{return false;}
float SMELT_IMPL::smChannelGetPosf(SMCHN chn)
{return -1.;}
void SMELT_IMPL::smChannelSetPosf(SMCHN chn,float pos)
{}
int SMELT_IMPL::smChannelGetPosd(SMCHN chn)
{return -1;}
void SMELT_IMPL::smChannelSetPosd(SMCHN chn,int pos)
{}

ALuint SMELT_IMPL::getSource()
{return 0;}
bool SMELT_IMPL::initOAL()
{
	smLog("%s:" SLINE ": I'm dumb!\n",SFX_SDL_SRCFN);
	pOpenALDevice=(void*)1;
	return true;
}
void SMELT_IMPL::finiOAL()
{
	if(pOpenALDevice)
	{
		pOpenALDevice=NULL;
	}
}
