# SMELT API documentation
(API Level 3)

## Table of contents
1. [Overview](#overview)
2. [History](#history)
3. [Macros](#macros)
4. [Data Types](#types)
5. [Core Functions](#core)
6. [Extensions](#extensions)
7. [Implementation details](#implementation)
8. [General considerations](#considerations)

## Overview <a name="overview"></a>
SMELT is a wrapper class for hardware-accelerated graphics rendering,
input handling and optionally, audio outputting. It works on X11, 
wayland and Windows. (Not tested on macOS because I don't use Mac.)

Currently, two implementations of SMELT exist. One uses SDL and the
other uses GLFW. The implementation using GLFW is further divided into
a OpenGL 1.2/2.1 version and a OpenGL 3.2 version.

## History <a name="history"></a>
SMELT is emerged from [hge-unix](https://icculus.org/hge-unix/), which
in turn was a port of HGE (Haaf's Game Engine) to MacOS X and UNIX.
Initially, I use hge-unix for my BLR series games. However starting from
an early testbed version of the still unreleased BLR 3, some 3D
rendering is required. So I added simple 3D scene rendering to hge-unix
and the original Windows version. I also wrote a TTF rendering module
for HGE and hge-unix. Finally, I decided to fork it.

After that, SMELT got new functionalities, new ports, and most
importantly, the code looks less messed up _to me_. However if you have
ever used HGE, you may still find the API of SMELT familiar.

## Macros <a name="macro"></a>
- `ARGB(a,r,g,b)`: Generates a hardware color with the specificed values.
- `RGBA(r,g,b,a)`: Same as ARGB, with different parameter order.
- `GET<A|R|G|B>(col)`: Extract alpha/red/green/blue value from a hardware color.
- `SET<A|R|G|B>(col,a|r|g|b)`: Set alpha/red/green/blue value of a hardware color.

- `PI`: `3.14159265358979323846f`

- `BLEND_ALPHAADD/BLEND_ALPHABLEND`: Alpha blending methods.
- `BLEND_COLORADD/BLEND_COLORMUL/BLEND_COLORINV`: Color blending methods.
- `BLEND_ZWRITE/BLEND_NOZWRITE`: Depth test options.

The `BLEND_*` macros of the different types can be or'd together.

- `FPS_FREE/FPS_VSYNC`: Special fps values.
- `INPUT_*`: Input event types.
- `PRIM_*`: Primitive types.
- `SMINP_*`: Key modifiers. These can be or'd together.
- `SMK_*`: Key scan codes.
- `SMKST_*`: Key states.
- `TPOT_*, TFLT_*`: Texture Options.

- `SMELT_APILEVEL`: SMELT api version.

## Data Types <a name="types"></a>
- `DWORD`: equivalent to `uint32_t`
- `WORD`: equivalent to `uint16_t`
- `BYTE`: equivalent to `uint8_t`
- `smHook`: (*bool)() function pointers used as callbacks.
- `SMCHN`: Represent a channel of sound playback.
- `SMSFX`: Represent a sound effect.
- `SMTEX`: Opaque texture type.
- `SMTRG`: Opaque render target type.

`smInpEvent`: Input event structure.

- `chcode`: Char code.
- `sccode`: Key scan code.
- `type`: Input event type. The macros `INPUT_*` are used.
- `flag`: Modifiers. Use bitwise and (`&`) with a `SMINP_*` value to
test for a modifier.
- `wheel`: Wheel motion.
- `x,y`: Mouse position.

`smVertex`: Vertex structure.

- `x,y,z`: Position. `z` can be used for depth testing in 2D mode.
- `col`: Vertex color. This color is in the format "AARRGGBB" (hex).
- `tx,ty`: Texture coordinates.

`smTriangle`: Triangle primitive structure.

- `v[3]`: Vertices of the triangle.
- `tex`: Texture of the triangle.
- `blend`: Blend mode of the triangle. The macros `BLEND_*` are used.

`smQuad`: Quadrilateral primitive structure.

- `v[4]`: Vertices of the quadrilateral.
- `tex`: Texture of the quadrilateral.
- `blend`: Blend mode of the quadrilateral. The macros `BLEND_*` are used.

WARNING: Quadrilaterals are treated as two triangles internally.
You may get unwanted results if the quadrilateral is not a parallelogram.

`smTexRect`: Texture rectangle sturcture.

- `x,y`: Top left coordinate of the texture used.
- `w,h`: Width & height of the texture used.

`smHandler`: Alternative handler class.

- `virtual bool handlerFunc()=0`

Implement the `handlerFunc()` and instances of this class can be used as
an alternative to `smHook` callbacks.

## Core Functions <a name="core"></a>
### External function

`SMELT* smGetInterface(int apilevel)`

Returns an interface to the SMELT core functions if the apilevel matches the
internal one, otherwise it returns NULL.

The internal facilities of SMELT are strectly singletons and this function
provides reference counting. Currently there is no way to use two or more
instances of SMELT at the same time in one application (unless you rename
the entire library).

### Functions inside the `SMELT` class

- `void smRelease()`  
Release one reference to the SMELT core. Once all references are released,
the internal SMELT instance will free all the resources associated with it.
**ALL pointers to the SMELT core generated before are invalidated!**

- `bool smInit()`  
Initialize the SMELT core.  
Creates application window, initializes video, audio and input handling.
No functions in these categories can be called before initialzing the
core.  
You might want to do some setup works before calling this.  
If the core is initialized successfully, this function returns true,  
otherwise it returns false. The errors are written to stderr and the log
file if set.

- `void smFinale()`  
Deinitialize the SMELT core. This function closes the application window
and frees all resources managed by SMELT.  
Unlike the behavior of `smRelease()` after the reference count reaching
zero, calling `smFinale()` retains the validity of all SMELT core pointers.
The core returns to the state prior to calling `smInit()`.

- `void smMainLoop()`  
Enters the main loop.  
The main loop handles window events (key press, close request, movement
and resize etc) and calls your update function regularly.  
This function requires the UpdateFunc set and initialization of the
core.

- `void smUpdateFunc(smHook func)`
- `void smUpdateFunc(smHandler* h)`  
Sets the update function.  
If the update function returns true, the main loop will break, otherwise
the main loop goes on.
The update function is called every frame.  
If both the handler and the hook are set, the hook will be executed first.

- `void smUnFocFunc(smHook func)`
- `void smUnFocFunc(smHandler* h)`  
Sets the unfocus event handler.
The unfocus function is called when the application window loses focus.  
If both the handler and the hook are set, the hook will be executed first.

- `void smFocFunc(smHook func)`
- `void smFocFunc(smHandler* h)`  
Sets the focus event handler.
The focus functions is called when the application window gains focus.  
If both the handler and the hook are set, the hook will be executed first.

- `void smQuitFunc(smHook func)`
- `void smQuitFunc(smHandler* h)`  
Sets the quit event handler.
Called when the user send a quit request to the application (e.g. by
pressing the close button).
If the handler/hook returns true, the quit request will be rejected and the
application will keep running.  
If both the handler and the hook are set, the hook will be executed first.

- `void smWinTitle(const char* title)`  
Sets the application window title.
The default title is "untitled".

- `bool smIsActive()`  
Returns true if the application window has focus. Otherwise returns false.

- `void smNoSuspend(bool para)`  
By default, the update function won't be called when the application
window doesn't have focus (`para=false`).
Call this with `para=true` to make SMELT keep running when the window is
not focused.

- `void smVidMode(int resX,int resY,bool _windowed)`  
Sets the application video mode.
The default mode is 800 x 600 fullscreen.  
Inappropriate values may cause the failure of smInit().  
In addition, only the `_windowed` parameter can be changed after the
creation of the window.

- `void smLogFile(const char* path)`  
Sets the log file path.

- `void smLog(const char* format,...)`  
Write something to the log file. C-style formatting can be used.  
The log will be written to stderr and the log file if set.

- `void smScreenShot(const char* path)`  
Takes a shot of the application window content and saves it to the
given path.  
Unimplemented in GLFW versions.

- `void smSetFPS(int fps)`  
Sets the desired FPS value.
The macros FPS_* can be used. Other values greater than 0 limits the
maximum FPS.

- `float smGetFPS()`  
Gets the current FPS value.
This value is updated once a second.

- `float smGetDelta()`  
Gets the delta time between the current frame and the last frame.

- `float smGetTime()`  
Gets the time in milliseconds since the call to `smInit()`.

- `SMSFX smSFXLoad(const char *path)`  
Loads a sound file from the given path. Currently only ogg vorbis and wav
files are supported.
~~Further more, due to the restrictions of openal-soft, only 8/16bit
mono/stereo formats are supported.~~ Seems not true anymore.  
**This function loads the entire decompressed sample into main memory,
and may take some time.** Fortunately for you, this function can be called
from another thread.

- `SMSFX smSFXLoadFromMemory(const char *ptr,DWORD size)`  
Loads a sound file from the given memory block.  
Limitations of `smSFXLoad()` also applies.

- `SMCHN smSFXPlay(SMSFX fx,int vol=100,int pan=0,float pitch=1.,bool loop=0)`  
Plays the given SFX. All parameters except the FX name is optional.  
A channel is generated and can be used to control the playing of the SFX.
The channel is valid until it stops.  
Volume should be in [0,100].  
Panning should be [-100,100].  
These values will be clamped to the range given above.  
Note: There's a limit of the total available channels. This is defined in
the header `smelt\_config.hpp` with the macro `SRC_MAX`. If you are playing
too many channels simultaneously or have too many channels paused, you
may exhaust all channels and cause this function to fail. If it fails,
the channel name equals to 0.

- `float smSFXGetLengthf(SMSFX fx)`  
Gets the sound length in seconds.

- `DWORD smSFXGetLengthd(SMSFX fx)`  
Gets the sound length in number of samples.

- `void smSFXSetLoopPoint(SMSFX fx,DWORD l,DWORD r)`  
Sets the loop point of a sound. The loop points will be used when the fx is
played with `loop=true`.
The unit of the parameters is #samples.
By default, the whole sample is looped.  
This function uses `AL_SOFT_loop_points` and may not work if SMELT isn't
built against OpenAl Soft.

- `void smSFXFree(SMSFX fx)`  
Releases the sound file from memory.
The handle will be invalid thereafter.

- `void smChannelVol(SMCHN chn,int vol)`  
Sets channel volume ([0,100]).

- `void smChannelPan(SMCHN chn,int pan)`  
Sets channel panning ([-100,100]).

- `void smChannelPitch(SMCHN chn,float pitch)`  
Sets channel pitch.

- `void smChannelPause(SMCHN chn)`  
Pauses the channel.

- `void smChannelResume(SMCHN chn)`  
Resumes a paused channel.

- `void smChannelStop(SMCHN chn)`  
Stops the channel. The channel handle will be invalid thereafter.

- `void smChannelPauseAll()`  
Pauses all playing channels.

- `void smChannelResumeAll()`  
Resumes all channels.

- `void smChannelStopAll()`  
Stops all channels.

- `bool smChannelIsPlaying(SMCHN chn)`  
Tests if a channel is playing.

- `float smChannelGetPosf(SMCHN chn)`
- `void smChannelSetPosf(SMCHN chn,float pos)`  
Gets/sets current position in second.

- `int smChannelGetPosd(SMCHN chn)`
- `void smChannelSetPosd(SMCHN chn,int pos)`  
Gets/sets current position in #sample.

- `void smGetMouse2f(float *x,float *y)`  
Gets mouse position (within the window).

- `void smSetMouse2f(float x,float y)`  
Sets mouse position (within the window).

- `void smSetMouseGrab(bool enabled)`  
Enables or disables mouse grabbing.
Useful for 3D camera control handling.

- `int smGetWheel()`  
Gets mouse wheel motion since the last frame.

- `bool smIsMouseOver()`  
Tests if the cursor is inside the application window.

- `int smGetKeyState(int key)`  
Gets the state of the given key. The `SMK_*` macros can be used.
Mouse buttons are treated as keys.   
The return values are one of the `SMKST_*` macros:
    * `SMKST_NONE`: the key is not pressed and is not just released either.
    * `SMKST_RELEASE`: the key is not pressed but is just released.
    * `SMKST_HIT`: the key is pressed but it was not pressed in the
    last frame.
    * `SMKST_KEEP`: the key is pressed and held since the last frame or
    earlier.

- `int smGetKey()`  
Gets the key pressed last frame.
If multiple keys are pressed, the last pressed one (the last one in the
event queue) counts.

- `bool smGetInpEvent(smInpEvent *e)`  
Gets a event from the input queue.
The input parameter will be set to point the event.  
If the input queue is empty, this function will return false.

- `bool smRenderBegin2D(bool ztest=0,SMTRG trg=0)`  
Starts rendering a 2D scene.  
Set `ztest` to true if you want to use the z coordinate to control
the layout.  
Set trg to the desired rendering target to render everything to the
target.  
The function resets the camera. You have to setup the camera with
`sm2DCamera5f3v`.  
NOTE: Any rendering function is not functional outside the
`smRenderBegin*D` function and its corresponding `smRenderEnd` function.

- `bool smRenderBegin3D(float fov,SMTRG trg=0)`  
Starts rendering a 3D scene.  
Depth test is always enabled in this mode.  
Set trg to the desired rendering target to render everything to the
target.  
The distances to clipping planes are hard-coded (0.1 and 1000). This may
change in a future revision.  
The function resets the camera. You have to setup the camera with
`sm3DCamera6f2v`.  
NOTE: Any rendering function is not functional outside the
`smRenderBegin*D` function and its corresponding `smRenderEnd` function.

- `bool smRenderEnd()`  
Ends rendering the scene.  
NOTE: Any rendering function is not functional outside the smRenderBegin*D
function and its corresponding smRenderEnd function.

- `void sm3DCamera6f2v(float *pos,float *rot)`  
Sets the 3D camera position and panning.  
Each float vector should countain three elements.  
The panning is euler rotation (all in degrees).  
If any of the vectors is `NULL`, the function will reset the camera
instead.  
The behaviour of the function is undefined in a 2D rendering session.

- `void sm2DCamera5f3v(float *pos,float *dpos,float *rot)`  
Sets the 2D camera transformation.  
`pos` should contain two elements for the camera's position.  
`dpos` should countain two elements for the rotation centre.  
`rot` is the pointer to a single float denoting the rotation (in degrees).
If any of the vectors is `NULL`, the function will reset the camera
instead.  
The behaviour of the function is undefined in a 3D rendering session.

- `void smMultViewMatrix(float *mat)`  
Provides direct manipulation on the model view matrix.
You can use "look at" matrix here, for example.  
The matrix the same as the matrix defined in smMatrix (column-major),
but stored in float (That's fairly stupid...).

- `void smClrscr(DWORD color,bool clearcol=true,bool cleardep=true)`  
Clears the screen/rendering target with color.  
Alpha channel is not applicable to the screen buffer.  
Depth buffer will only be cleared if cleardep is set to true.
The same applies to the color buffer.

- `void smRenderLinefd(float x1,float y1,float z1,float x2,float y2,float z2,DWORD color)`  
Renders a line from (x1,y1,z1) to (x2,y2,z2) in the given color.
Lines never have textures.

- `void smRenderLinefvd(float *p1,float *p2,DWORD color)`  
Renders a line from (p1[0],p1[1],p1[2]) to (p2[0],p2[1],p2[2]) in the
given color.

- `void smRenderTriangle(smTriangle *t)`  
Renders a triangle.

- `void smRenderQuad(smQuad *q)`  
Renders a quadrilateral.

- `smVertex* smGetVertArray()`  
Return a pointer to the internal vertex array for advanced drawing.
The vertex array is rendered and cleared before returning it to you.
So you will always get the whole vertex array.  
After filling the vertex array, call `smDrawVertArray()` to draw them.  
By default, the size of the vertex array is 4000. This is defined in the
header `smelt_config.hpp` with the macro `VERTEX_BUFFER_SIZE`.

- `void smDrawVertArray(int prim,SMTEX texture,int blend,int _primcnt)`  
Draws the vertex array modified by `smGetVertArray()`.
The first parameter represents the type of primitive to be drawn. You can
use the macros `PRIM_*` here.

- `void smDrawCustomIndexedVertices(smVertex* vb,WORD* ib,int vbc,int ibc,int blend,SMTEX texture)`  
Draw indexed vertices. The primitive type is always triangle.  
This function basically allows you to pass custom indices to
`glDrawElements()` instead the built in ones.
If you are not quite sure about what this function does, just ignore it.
    - `vb`: pointer to vertices to be drawn.
	- `ib`: pointer to indices of the vertices.
	- `vbc`: number of vertices.
	- `ibc`: number of indices.
	- `blend`: blending mode.
	- `texture`: texture used for the drawing.  

- `SMTRG smTargetCreate(int w,int h,int ms=0)`  
Creates a rendering target (sized `w*h`) with multisampling level ms.  
Modern OpenGL supports non-power-of-two sized targets. However some legacy
hardware may lack the support.
In this case, the target will be automatically enlarged.  
Multisampling is disabled if `ms=0`. This value is only a hint to SMELT.
If multisampling is not supported, it will be disabled automatically.

- `SMTEX smTargetTexture(SMTRG targ)`  
Gets the texture of the rendering target. The texture contains the resulting
image rendered to that target.

- `void smTargetFree(SMTRG targ)`  
Frees the rendering target.

- `SMTEX smTextureCreate(int w,int h)`  
Creates a blank texture (sized `w*h`).
Modern OpenGL supports non-power-of-two sized textures. However some legacy
hardware may lack the support.
In this case, the texture will be automatically enlarged.

- `SMTEX smTextureLoad(const char *path,bool mipmap=false)`  
Loads texture from the given file.
The POT (power-of-two) issue above also applies here.  
Mipmapping is not implemented yet.

- `SMTEX smTextureLoadFromMemory(const char *ptr,DWORD size,bool mipmap=false)`  
Loads texture from the given memory block.
See also `smTextureLoad()`.

- `void smTextureFree(SMTEX tex)`  
Release the texture from memory.

- `void smTextureOpt(int potopt=TPOT_NONPOT,int filter=TFLT_LINEAR)`  
Sets texture options.
    - `TPOT_POT`: textures dimensions are _always_ resized to the minimal
	power of two value. In this mode, textures repeat themselves if the
	texture coordinate is greater than one (`GL_REPEAT` is used).
    - `TPOT_NONPOT`: use textures whose dimensions are not power of
	two directly. If unsupported by the hardware, textures will
	be resized anyway. This turns off texture repeating.
    - `TFLT_LINEAR`: use the linear filter for texture scaling.
    - `TFLT_NEAREST`: use the nearest filter for texture scaling.

- `int smTextureGetWidth(SMTEX tex,bool original=false)`  
Gets the width of the texture.  
If `original==false` and the texture is resized, the function will return
the resized POT width of the texture.
Otherwise it returns the actual width of the texture file.

- `int smTextureGetHeight(SMTEX tex,bool original=false)`  
Gets the height of the texture.  
If `original==false` and the texture is resized, the function will return
the resized POT height of the texture.
Otherwise it returns the actual height of the texture file.

- `DWORD* smTextureLock(SMTEX tex,int l,int t,int w,int h,bool ro=true)`  
Locks the texture for reading/writing.  
The locked area is defined as `(l,t,w,h)`: left, top, width, height.
if `ro==true`, the changes won't be uploaded back to the video memory.
Textures of rendering targets cannot be locked.  
Currently, textures are locked **upside-down** -- this is due to the fact
that textures in OpenGL originate from bottom-left. However the `top`
parameter really represents what its name means. This is insane but I've
got used to it. AND the locked pixels are in `AABBGGRR` format with
`AA` at the side of the most significant bit, which makes everything
even better.

- `void smTexutreUnlock(SMTEX tex)`  
Unlocks the texture so that it can be used for rendering.
The changes will only be commited to the video memory if `ro==false` when
locking the texture.

## Extensions <a name="extensions"></a>

### smAnim
This extension provides texture management and animated entity support.

### smBMFont
This extension provides bitmap font support.

### smColor
This extension provides conversion between color models.

### smDataPack
This extension provides managed resources loading/writing.

### smEntity
This extension provides easy entity rendering methods.

### smGrid
This extension allows you to create distortion using a grid.

### smIndicator
This extension implements several value indicators.

### smMath
This extension implements common math routines.

### smProgresser
This extension implements interpolation.

### smRandom
This extension implements a pseudo random number generator.

### smTTFont
This extension implements truetype font rendering.

## Implementation details <a name="implementation"></a>

- The OpenGL implementation uses a client-side vertex array.
All `smRender*()` rendering commands are buffered here before
a draw has to be done. The size of the vertex array can be
adjusted by the `VERTEX_BUFFER_SIZE` macro, found in `smelt_config.hpp`.
- A draw batch has to be done when at least one of these changes:
    - Primitive type
	- Texture
	- Blending mode
  or when...
    - the vertex array is full
	- `smRenderEnd()` is called
	- changing camera properties
	- using 'advanced' drawing functions (`smGetVertArray()`,
	`smDrawCustomIndexedVertices`)
- When doing a batch draw, SMELT uploads the _whole_ vertex array
(actually only the portion with your data) to OpenGL. So there's no
real need for a vertex buffer object, but it hurts if you draw a lot
of static objects.
- The sound system implementation puts a limit on the number of effects
played at the same time. This is defined by the `SRC_MAX` macro in
`smelt_config.hpp`.
- Currently there's no way to use two or more instances of SMELT in one
application at the same time.

## General considerations <a name="considerations"></a>
- For the sake of performance, keep the number of draw batches as low as
possible. Avoid switching properties that will trigger a batch too
frequently.
- ~~Use profiling functions to help improve performance.~~ Profiling
functions are not yet added.
- If you are going to draw something with plenty of coinciding vertices,
consider using `smDrawCustomIndexedVertices()`.

Old junk starting from here.

smAnimation====================================================================
Texture info class: smTexInfo
smTexRect rect;							Texture rectangle
char *name,*path;						name and internal path of the texture
SMTEX tex;								Texture handle

Animation info class: smAnmInfo
smTexInfo frames[256]					Frames of the animation
int framedur[256]						Frame durations
int mode								loop mode: 0=noloop 1=loop 2=bidi loop
int fc									Frame count
char* name								Name of the animation

Animation file class: smAnmFile
For the format of the .anm file, please refer to the file datapackFormat.
-bool loadAnmFromMemory(char* ptr,DWORD size)
 Loads a .anm file from the given memory block.
 .anm file is always packaged in dtp files in practice so only the load from
 memory version is implemented.
 All textures are loaded into the video memory during the process, so make sure
 SMELT is initialized before calling this function.
-void close()
 Close the anm file, free all textures and do the cleanup.
-smTexInfo* getTextureInfo(const char* name)
 Gets the details of the texture with the given name.
-smAnmInfo* getAnimationInfo(const char* name)
 Gets the details of the animation with the given name.

2D animated entity class: smAnimation2D
smAnimation2D is derived from the smEntity2D class.
-Constructor from a smAnmInfo class
-void updateAnim(int f=1)
 Updates the animation with frame step f.
-void resetAnim()
 Resets the animation.

3D animated entity class: smAnimation3D
smAnimation3D is derived from the smEntity3D class.
-Constructor from a smAnmInfo class
-void updateAnim(int f=1)
 Updates the animation with frame step f.
-void resetAnim()
 Resets the animation.
smBMFont=======================================================================
The extension provides support of bitmap font rendering.
Bitmap font used by SMELT is described in anm format, with a different meta
file.

Two classes are provided: smBMFont and smBMFontw. smBMFont accepts char stings
and smBMFontw accepts wchar strings. Their interfaces are the same.

smBMFontw uses std::map, so huge font files are discouraged.

Public members of smBMFont(w):
-loadAnmFromMemory(char* ptr,DWORD size)
 Loads an anm file describing the font from the given memory block.
-close()
 Closes the anm file and frees any resources used by the class.
-render(float x,float y,float z,int align,float *rw,const char*/wchar_t* text)
 Render the given text at (x,y,z) with the given align.
 ailgns can be one of the macros ALIGN_LEFT/ALIGN_RIGHT/ALIGN_CENTER.
 if rw is not NULL, the width of the string will be returned to the pointer.
 '\n' is processed correctly. Other control characters are not processed and
 may not render correctly.
-printf(float x,float y,float z,int align,float *rw,const wchar_t* format,...)
 Render formated string at (x,y,z) with the given align.
 The parameters are the same as the render function.
-setColor(DWORD col)
 Sets the color for the next rendering.
-setBlend(int blend)
 Sets blending mode for the next rendering.
-setScale(int scale)
 Sets the font scaling.

smColor========================================================================
The extension implements two color classes: smColorRGBA and smColorHSVA.

Public members of smColorRGBA:
-float r,g,b,a							R/G/B/A components, ranged 0..1
-Default construction op				Sets all components to zero
-Construction op from a hardware color
-Construction op from components
-Default destuction op
-void clamp()							clamps all components to their range
-operator + - *							color operations
-operator * /							scalar operations
-void setHWColor(DWORD col)				Sets the color accroding to the given
										hardware color
-DWORD getHWColor()						Gets the hardware color. The color is
										automatically clamped.

Public members of smColorHSVA:
-float h,s,v,a							R/G/B/A components, ranged 0..1
-Default construction op				Sets all components to zero
-Construction op from a hardware color
-Construction op from components
-Default destuction op
-void clamp()							clamps all components to their range
-operator + - *							color operations
-operator * /							scalar operations
-void setHWColor(DWORD col)				Sets the color accroding to the given
										hardware color
-DWORD getHWColor()						Gets the hardware color. The color is
										automatically clamped.

smDataPack=====================================================================
The extension implements a data packaging method for increased integrity.
smDtpFileR is used to read a dtp file, while smDtpFileW is used to create a 
dtp file.
The format of the dtp file is described in the file datapackFormat.

Public members of smDtpFileR:
-bool openDtp(const char* path)
 Opens a compressed dtp from the given path.
 Returns true on succeed, false on failure.
-bool openDtpFromMemory(char* ptr,DWORD size)
 Opens a uncompressed dtp from the memory block.
 Returns true on succeed, false on failure.
-void closeDtp()
 Close the opened dtp file. Release any internal allocated resources.
-char* getFirstFile()
 Returns the internal path of the first file in the dtp.
-char* getLastFile()
 Returns the internal path of the last file in the dtp.
-char* getNextFile(const char* path)
 Returns the internal path of the next file in the dtp.
-char* getPrevFile()
 Returns the internal path of the previous file in the dtp.
-char* getFilePtr(const char* path)
 Get the pointer to the file required.
 The file is loaded into memory during the process.
-void releaseFilePtr(const char* path)
 Release the file in the memory.
 Always free the file with this function.
-DWORD getFileSize(const char* path)
 Get the file size in bytes of the file required.

Public members of smDtpFileW:
-bool addFile(const char* path,const char* realpath)
 Adds a file to the temporary dtp.
 path is used as the internal path, realpath is the path of the data
 to be packed.
-bool writeDtp(const char* path)
 Writes the temporary dtp to a file.
 The dtp file is compressed.
 The temporary dtp file is deleted after the process.

A simple DaTaPack utility implementation is in the folder dtputil.
smEntity=======================================================================
The extension implements two entity classes for easy entity rendering.

Public members of smEntity2D:
-Consturction op from texture and
 texture rectangle(SMTEX,float,float,float,float)
-Construction op from texture and
 texture rectangle(SMTEX,smTexRect)
-Construction op from another smEntity2D
-Destuction op
-void Render(float x,float y,float rot,float wsc,float hsc)
 Renders the entity at (x,y) with rotation rot (optional, in radians).
 wsc/hsc (optional) controls the scaling in width and height.
 if hsc is omitted, wsc is used as hsc. By default, wsc=1.
-void setTexutre(SMTex tex)
 Sets the texture of the entity.
-void setTextureRect4f(float _x,float _y,float _w,float _h)
 void setTextureRectv(smTextRect rect)
 Sets the area of texture used for rendering.
-void setColor(DWORD col,int v)
 Sets the color of the entity or one of the vertexes of the entity.
 If v is omitted or v is out of the range 0..3, all four vertexes are
 re-colored.
-void setZ(float z,int v)
 Sets the z value of the entity or one of the vertexes of the entity.
 If v is omitted or v is out of the range 0..3, all four vertexes are
 re-positioned.
-void setBlend(int blend)
 Sets the blend mode of the entity. The macros BLEND_* are used.
-void setCentre(float x,float y)
 Sets the centre of the entity. The centre is used as the rotation centre.
 When rendered, the centre is used to position the entity.

//There's a pending overhaul of smEntity3D. Please ignore the section below.
smEntity3D is almost the same as smEntity2D, except the lack of setZ function
and some difference in the rendering methods. Further more, smEntity3D always
writes to the Z buffer when rendered.
-void Render9f(float x,float y,float z,float ra,float rx,float ry,float rz,
 float wsc,float hsc)
 Renders the 3D entity at (x,y,z).
 The entity is rotated by the parameters (ra,rx,ry,rz), in which ra specifies
 the angle of rotation in radians and (rx,ry,rz) specify the rotation axis.
 The rotation parameters are optional. The vector (rx,ry,rz) is automatically
 normalized.
 wsc/hsc (optional) controls the scaling in width and height.
 if hsc is omitted, wsc is used as hsc. By default, wsc=1.
-void Renderfv(float* pos,float* rot,float* scale)
 Equivalent to Render9f(pos[0],pos[1],pos[2],rot[0],rot[1],rot[2],rot[3]
 scale[0],scale[1]).

smGrid=========================================================================
The extension implements a grid used for rendering distorted entity.
Sample grid:
```
+-----+-----+
|     |     |
|     |     |
|     |     |
+-----+-----+
|     |     |
|     |     |
|     |     |
+-----+-----+
```
This is a simple 3*3 grid, undistorted. If we fit a texture onto it we will
see a rectangle with that texture.
Now we can distort it like this:
```
+-----+-----+
 \     \     \
  \     \     \
   \     \     \
    +-----+-----+
     \     \     \
      \     \     \
       \     \     \
        +-----+-----+
```
If we fit a texture onto this grid, we will see a parallelogram.
The grid looks better when the resolution is higher.
With this extension we can implement water/lens/wrapping effect and curved
lasers easily.
The indicatorCircular in the smIndicator extension is also implemented with
smGrid.

Public members of smGrid:
-Default construction op (int,int)
 This construction op requires the column&row count of the grid.
-Copy construction op(const smGrid &copy)
 Constructs a new smGrid copying the given one.
-Default destuction op.
-operator =
-void render(float x,float y)
 Renders the grid at (x,y). (x,y) is the top left coordinate.
-void clear(DWORD color)
 Resets the grid with the optional color.
-void setTexutre(SMTEX tex)
 Sets the texture of the grid.
-void setTextureRect4f(float _x,float _y,float _w,float _h)
 void setTextureRectv(smTexRect rect)
 Sets the texture region used by the grid.
-void setBlend(int blend)
 Sets the blend mode of the grid.
-void setColor(int c,int r,DWORD col)
 Sets the color of the node at row r column c.
 You can create interesting colorful effects with this.
-void setPos(int c,int r,float x,float y,float z,int ref)
 Sets the positioning of the node at row r column c.
 The z value is always directly set.
 The x/y values are set according the reference setting.
 Possible references are:
 GRID_REFNODE: reference point is the node position of a regular grid
 GRID_REFTOPLEFT: reference point is the top left corner of the grid
 GRID_REFCENTER: reference point is the centre of the grid
 GRID_REFCURRENT: reference point is the current position of the node

smIndicator====================================================================
The extension implements several value indicators.

Public members of class indicatorCircular/indicatorLinear:
-void init(double r,double thkns,BYTE a,SMTEX tex,smTexRect tr)
 Initialize the indicator with radius r, thickness thkns.
 a is used as the initial alpha.
-void overrideColor(DWORD col)
 Overrides the default dynamic color with the static color.
-void setAlpha(BYTE a)
 Sets the indicator alpha
-void setValue(double v)
 Sets the value indicated by the indicator.
 The value should be within 0..1.

smMath=========================================================================
The extension provides several math routines.

Common math routines:
double smMath::deg2rad(double deg)		converts degree to radian
double smMath::rad2deg(double deg)		converts radian to degree
double smMath::clamprad(double a)		clamps the given angle to [0,2*PI]
double smMath::clampdeg(double a)		clamps the given angle to [0,360]

2D Vector: smvec2d
double x,y								coordinates
Constructor(double,double)				constructs the vector with the given
										coordinates
Default constructor
double l()								returns the length of the vector
void normalize()						normalizes the vector
smvec2d getNormalized()					returns the normalized vector
void swap()								swaps the x,y coordinates
void rotate(double rad)					rotate the vector by rad radians
smvec2d getRotate(double rad)			returns the rotated vector
smvec2d operator - +
smvec2d/double operator *				scalar/(length of) cross product
double operator |						point product
double operator ^						angle between the vectors

3D Vector: smvec3d
double x,y,z							coordinates
Constructor(double,double,double)		constructs the vector with the given
										coordinates
Constructor(smvec2d)					constructs the vector from the 2d
										vector with z=0
Default constructor
double l()								returns the length of the vector
void normalize()						normalizes the vector
smvec3d getNormalized()					returns the normalized vector
smvec3d operator - +
smvec3d operator *						scalar/cross product
smvec3d operator |						point product
double operator ^						angle between the vectors

Transformation matrix: smMatrix
double m[16]
sf  0  1  2  3
0  00 04 08 12
1  01 05 09 13
2  02 06 10 14
3  03 07 11 15
Default constructor
Copy constructor
operator []								with this you can access the matrix
										like (smMatrix)a[col][row]
void clear()							sets the whole matrix to 0
void loadIdentity()						sets the matrix to the identity matrix
void rotate(double,double,double,double)
										rotates the vector by (a,x,y,z) where
										a is the rotation angle in radians and
										(x,y,z) is the rotation axis.
void lookat(double *eye,double *at,double *up)
										all vectors have three elements.
										This function produces a "look at"
										matrix.
										When the identity view matrix is
										multiplied by the "look at" matrix, it
										seems that you are at the "eye"
										position, looking at the "at" opsition.
										The up vector is used to determine the
										pitch of the camera.
operator *								Matrix-matrix multiplication.
operator *								Matrix-vector multiplication.
smProgresser===================================================================
The extension implements several progressers useful for making transitions.
All progressers work in the same way: it returns a value within [a,b] according
to the current progress.

All progresser have these functions publicly accessible:
void init(double _a,double _b,double _lim,[additional parameters])
Initializes the progresser with a=_a, b=_b and maximum progress _lim.
void launch()
Launch the progresser. This function actually resets the progresser.
void update(double delta)
Updates the progress with delta value delta.
bool isDone()
Check if the progresser has completed its progress.
double getValue()
Gets the current progresser value.
double getPercentage()
Gets the current progress percentage(always linear).
double getDelta()
Gets the current delta between progresser value and a.
double getElapsed()
Gets the current elapsed progress.

Currently two types of progressers are implemented.
-smProgresserLinear
Normal linear progresser.
-smProgresserCurve
Curve progresser. This progresser requires an additional parameter to
Initialize, which is the radius of the curve. The bigger abs(curvepara) is,
the steeper the progressing curve will be. If curvepara=0, it will act
as the linear progresser. The sign of the parameter controls the bending
direction of the curve.

smRandom=======================================================================
The extension implements a pseudo random number generator.
Pseudo random number generator always generates the same sequence of number
provided that the seed and the calling sequence is the same.

Public members of smRandomEngine
void setSeed(unsigned int seed)
Sets the random seed. The seed affects the next number generated.
int nextInt(int min,int max)
Generate a random integer. The seed is updated too.
Please note that max is not necessarily greater than min.
double nextDouble(double min,double max)
Generate a random double. The seed is updated too.
Please note that max is not necessarily greater than min.
smTTFont=======================================================================
The extension implements a freetype interface for SMELT.
Two classes are implemented: smTTChar and smTTFont. smTTChar is implemented for
internal use and should not be used outside the smTTFont class. Class smTTFont
provides all interfaces for font rendering.

smTTFont uses std::map to cache character glyphs. The cache is manually managed
by the application. A cache too large in size may cause lags.

Public members of smTTFont:
-bool loadTTF(const char* path,int pt)
 Loads a font from the given path. pt is used as the size of the font in
 points.
-bool loadTTFFromMemory(char* ptr,DWORD size,int pt)
 Loads a font from the given memory block. pt is used as the size of the font
 in points.
-void releaseTTF()
 Clear the cache and release the loaded font.
-float getWidth()
-float getHeight()
 Gets the width/height of the buffered string.
 The buffered string is modified by the updateString function.
-void updateString(const wchar_t *format,...)
 Update the buffered string.
 Update the buffer only when you need to do so because this function is
 relatively slow.
 '\n' is processed correctly.
 If a char is in the internal cache, it will be reused. Otherwise it will be
 added to the cache.
-void render(float x,float y,DWORD col,int align)
 Renders the buffered string.
 The align mode ALIGN_CENTER is not supported.
 (x,y) defines the top-left coordinate of the rendered text when using
 ALIGN_LEFT while it defines the bottom-right coordinate when using
 ALIGN_RIGHT.
 col defines the color of the rendered text.
-DWORD getCacheSize()
 Gets the internal cache size.
 It is advised to keep the cache size relatively low, generally below
 100 thousand.
-void clearCache()
 Clear the internal cache.
