// -*- C++ -*-
/*
 * Simple MultimEdia LiTerator(SMELT)
 * by Chris Xiong 2015
 * GFX implementation based on GLFW/OpenGL 3.2+
 *
 * WARNING: This library is in development and interfaces would be very
 * unstable.
 *
 */
#include "smelt_internal.hpp"
#include "CxImage/ximage.h"
#define dbg printf("%d: 0x%X\n",__LINE__,glGetError())
#ifndef USE_OPENGL_COMPATIBILITY_PROFILE
#include "smmath_priv.hpp"
static const char* GFX_GLFW_SRCFN="smelt/glfw/gfx_glfw.cpp";
static const char* fixedfunc_pipeline_vsh=
	"#version 330 core\n"
	"layout (location=0) in vec3 vp;"
	"layout (location=1) in vec4 vc;"
	"layout (location=2) in vec2 vtc;"
	"out vec4 fc;"
	"out vec2 ftc;"
	"uniform mat4 mmodv;"
	"uniform mat4 mproj;"
	"void main(){"
		"gl_Position=mproj*mmodv*vec4(vp,1.0f);"
		"ftc=vec2(vtc.x,1.0f-vtc.y);"
		"fc.rgba=vc.bgra;"
	"}"
;
static const char* fixedfunc_pipeline_fsh=
	"#version 330 core\n"
	"in vec4 fc;"
	"in vec2 ftc;"
	"out vec4 color;"
	"uniform sampler2D tex;"
	"void main(){"
		"color=fc*texture(tex,ftc);"
		"if(color.a<1./256.)discard;"
	"}"
;
struct glTexture
{
	GLuint name,rw,rh,dw,dh;
	const char *fn;
	DWORD *px,*locpx;
	bool isTarget,lost,roloc;
	GLint locx,locy,locw,loch;
};
bool SMELT_IMPL::smRenderBegin2D(bool ztest,SMTRG trg)
{
	TRenderTargetList *targ=(TRenderTargetList*)trg;
	if(vertexArray)
	{smLog("%s:" SLINE ": Last frame not closed.\n",GFX_GLFW_SRCFN);return false;}
	glBindFramebuffer(GL_FRAMEBUFFER,(targ)?targ->frame:0);
	glDepthFunc(GL_GEQUAL);
	ztest?glEnable(GL_DEPTH_TEST):glDisable(GL_DEPTH_TEST);
	zbufenabled=ztest;
	if(targ)
	{
		glScissor(0,0,targ->w,targ->h);
		glViewport(0,0,targ->w,targ->h);
		configProjectionMatrix2D(targ->w,targ->h);
	}
	else
	{
		glScissor(0,0,scrw,scrh);
		glViewport(0,0,scrw,scrh);
		configProjectionMatrix2D(scrw,scrh);
	}
	sm2DCamera5f3v(NULL,NULL,NULL);
	curTarget=targ;tdmode=0;
	vertexArray=vertexBuf;
	return true;
}
bool SMELT_IMPL::smRenderBegin3D(float fov,bool ztest,SMTRG trg)
{
	TRenderTargetList *targ=(TRenderTargetList*)trg;
	if(vertexArray)
	{smLog("%s:" SLINE ": Last frame not closed.\n",GFX_GLFW_SRCFN);return false;}
	glBindFramebuffer(GL_FRAMEBUFFER,(targ)?targ->frame:0);
	glDepthFunc(GL_LESS);
	ztest?glEnable(GL_DEPTH_TEST):glDisable(GL_DEPTH_TEST);
	zbufenabled=ztest;
	if(targ)
	{
		glScissor(0,0,targ->w,targ->h);
		glViewport(0,0,targ->w,targ->h);
		configProjectionMatrix3D(targ->w,targ->h,fov);
	}
	else
	{
		glScissor(0,0,scrw,scrh);
		glViewport(0,0,scrw,scrh);
		configProjectionMatrix3D(scrw,scrh,fov);
	}
	sm3DCamera6f2v(NULL,NULL);
	curTarget=targ;tdmode=1;
	vertexArray=vertexBuf;
	return true;
}
bool SMELT_IMPL::smRenderEnd()
{
	batchOGL(true);
	if(curTarget&&curTarget->ms)
	{
		glTexture *pTex=(glTexture*)curTarget->tex;
		if(pTex&&pTex->lost)
		configTexture(pTex,pTex->rw,pTex->rh,pTex->px);
		int w=curTarget->w,h=curTarget->h;
		glFinish();
		DWORD *px=new DWORD[w*h];
		glBindFramebuffer(GL_READ_FRAMEBUFFER,curTarget->frame);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER,curTarget->sframe);
		glBlitFramebuffer(0,0,w,h,0,0,w,h,GL_COLOR_BUFFER_BIT,GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER,curTarget->sframe);
		glReadPixels(0,0,w,h,GL_RGBA,GL_UNSIGNED_BYTE,px);
		glBindTexture(GL_TEXTURE_2D,pTex->name);
		glTexSubImage2D(GL_TEXTURE_2D,0,0,0,w,h,GL_RGBA,GL_UNSIGNED_BYTE,px);
		glBindTexture(GL_TEXTURE_2D,primTex?(((glTexture*)primTex)->name):0);
		delete[] px;
	}
	if(!curTarget)glfwSwapBuffers((GLFWwindow*)hwnd);
	return true;
}
void SMELT_IMPL::smClrscr(DWORD color,bool clearcol,bool cleardep)
{
	GLfloat a=(GLfloat)(GETA(color))/255.f;
	GLfloat r=(GLfloat)(GETR(color))/255.f;
	GLfloat g=(GLfloat)(GETG(color))/255.f;
	GLfloat b=(GLfloat)(GETB(color))/255.f;
	if(clearcol)
	glClearColor(r,g,b,a);
	if(tdmode)glClearDepth(1);
	else if(zbufenabled)glClearDepth(0);
	glClear((clearcol?GL_COLOR_BUFFER_BIT:0)|(((zbufenabled||tdmode)&&cleardep)?GL_DEPTH_BUFFER_BIT:0));
}
void SMELT_IMPL::sm3DCamera6f2v(float *pos,float *rot)
{
	batchOGL();
	_smMatrix Mmodv;
	Mmodv.loadIdentity();
	if(pos&&rot)
	{
		Mmodv.rotate(_smMath::deg2rad(-rot[0]),1,0,0);
		Mmodv.rotate(_smMath::deg2rad(-rot[1]),0,1,0);
		Mmodv.rotate(_smMath::deg2rad(-rot[2]),0,0,1);
		Mmodv.translate(-pos[0],-pos[1],-pos[2]);
	}
	memcpy(mmodv,Mmodv.m,sizeof(mmodv));
	glUniformMatrix4fv(loc_mmodv,1,0,mmodv);
}
void SMELT_IMPL::smMultViewMatrix(float *mat)
{
	_smMatrix Mmodv(mmodv);
	Mmodv=Mmodv*_smMatrix(mat);
	memcpy(mmodv,Mmodv.m,sizeof(mmodv));
}
void SMELT_IMPL::sm2DCamera5f3v(float *pos,float *dpos,float *rot)
{
	batchOGL();
	_smMatrix Mmodv;
	Mmodv.loadIdentity();
	if(pos&&dpos&&rot)
	{
		Mmodv.translate(-pos[0],-pos[1],.0f);
		Mmodv.rotate(*rot,.0f,.0f,1.0f);
		Mmodv.translate(pos[0]+dpos[0],pos[1]+dpos[1],.0f);
	}
	memcpy(mmodv,Mmodv.m,sizeof(mmodv));
	glUniformMatrix4fv(loc_mmodv,1,0,mmodv);
}
void SMELT_IMPL::smRenderLinefd(float x1,float y1,float z1,float x2,float y2,float z2,DWORD color)
{
	if(vertexArray)
	{
		if(primType!=PRIM_LINES||primcnt>=VERTEX_BUFFER_SIZE/PRIM_LINES||primTex||primBlend!=BLEND_ALPHABLEND)
		{
			batchOGL();
			primType=PRIM_LINES;
			if(primBlend!=BLEND_ALPHAADD)setBlend(BLEND_ALPHAADD);
			bindTexture(NULL);
		}
		int i=(primcnt++)*PRIM_LINES;
		vertexArray[i].x=x1,vertexArray[i+1].x=x2;
		vertexArray[i].y=y1,vertexArray[i+1].y=y2;
		vertexArray[i].x=z1,vertexArray[i+1].x=z2;
		vertexArray[i].col=vertexArray[i+1].col=color;
		vertexArray[i].tx=vertexArray[i+1].tx=.0;
		vertexArray[i].ty=vertexArray[i+1].ty=.0;
	}
}
void SMELT_IMPL::smRenderLinefvd(float *p1,float *p2,DWORD color)
{
	if(vertexArray&&p1&&p2)
	{
		if(primType!=PRIM_LINES||primcnt>=VERTEX_BUFFER_SIZE/PRIM_LINES||primTex||primBlend!=BLEND_ALPHABLEND)
		{
			batchOGL();
			primType=PRIM_LINES;
			if(primBlend!=BLEND_ALPHAADD)setBlend(BLEND_ALPHAADD);
			bindTexture(NULL);
		}
		int i=(primcnt++)*PRIM_LINES;
		vertexArray[i].x=p1[0],vertexArray[i+1].x=p2[0];
		vertexArray[i].y=p1[1],vertexArray[i+1].y=p2[1];
		vertexArray[i].x=p2[2],vertexArray[i+1].x=p2[2];
		vertexArray[i].col=vertexArray[i+1].col=color;
		vertexArray[i].tx=vertexArray[i+1].tx=.0;
		vertexArray[i].ty=vertexArray[i+1].ty=.0;
	}
}
void SMELT_IMPL::smRenderTriangle(smTriangle *t)
{
	if(vertexArray)
	{
		if(primType!=PRIM_TRIANGLES||primcnt>=VERTEX_BUFFER_SIZE/PRIM_TRIANGLES||
		primTex!=t->tex||primBlend!=t->blend)
		{
			batchOGL();
			primType=PRIM_TRIANGLES;
			if(primBlend!=t->blend)setBlend(t->blend);
			bindTexture((glTexture*)t->tex);
		}
		memcpy(&vertexArray[(primcnt++)*PRIM_TRIANGLES],t->v,sizeof(smVertex)*PRIM_TRIANGLES);
	}
}
void SMELT_IMPL::smRenderQuad(smQuad *q)
{
	if(vertexArray)
	{
		if(primType!=PRIM_QUADS||primcnt>=VERTEX_BUFFER_SIZE/PRIM_QUADS||
		primTex!=q->tex||primBlend!=q->blend)
		{
			batchOGL();
			primType=PRIM_QUADS;
			if(primBlend!=q->blend)setBlend(q->blend);
			bindTexture((glTexture*)q->tex);
		}
		memcpy(&vertexArray[(primcnt++)*PRIM_QUADS],q->v,sizeof(smVertex)*PRIM_QUADS);
	}
}
smVertex* SMELT_IMPL::smGetVertArray()
{
	if(vertexArray)
	{
		batchOGL();
		return vertexArray;
	}
	else return NULL;
}
void SMELT_IMPL::smDrawVertArray(int prim,SMTEX texture,int blend,int _primcnt)
{
	primType=prim;
	bindTexture((glTexture*)texture);
	if(primBlend!=blend)setBlend(blend);
	primcnt=primcnt;
}
void SMELT_IMPL::smDrawCustomIndexedVertices(smVertex* vb,WORD* ib,int vbc,int ibc,int blend,SMTEX texture)
{
	if(vertexArray)
	{
		batchOGL();

		float twm=-1.,thm=-1.;
		if(texture)
		{
			glTexture *ptex=(glTexture*)texture;
			if(ptex->dw&&ptex->dh)
			{
				twm=(ptex->rw)/(float)(ptex->dw);
				thm=(ptex->rh)/(float)(ptex->dh);
			}
		}
		if(!tdmode)
		{
			float h=curTarget?curTarget->h:scrh;
			for(int i=0;i<vbc;++i)
			{
				vertexArray[i].y=h-vertexArray[i].y;
				vertexArray[i].z=-vertexArray[i].z;
			}
		}
		if(twm>0&&thm>0)
		for(int i=0;i<vbc;++i)
		{
			vb[i].tx*=twm;
			vb[i].ty*=thm;
		}
		if(texture!=primTex)bindTexture((glTexture*)texture);
		if(blend!=primBlend)setBlend(blend);
		glBindVertexArray(VertexArrayObject);
		glBindBuffer(GL_ARRAY_BUFFER,VertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER,sizeof(smVertex)*vbc,vb,GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,IndexBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLushort)*ibc,ib,GL_DYNAMIC_DRAW);
		glDrawElements(GL_TRIANGLES,ibc,GL_UNSIGNED_SHORT,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,IndexBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLushort)*((VERTEX_BUFFER_SIZE*6)/4),indexBuf,GL_DYNAMIC_DRAW);
		glBindVertexArray(0);
		if(texture!=primTex)bindTexture((glTexture*)primTex);

	}
}
SMTRG SMELT_IMPL::smTargetCreate(int w,int h,int ms)
{
	bool ok=false;
	TRenderTargetList *pTarget=new TRenderTargetList;
	memset(pTarget,0,sizeof(TRenderTargetList));
	pTarget->tex=buildTexture(w,h,NULL);
	glTexture *gltex=(glTexture*)pTarget->tex;
	gltex->isTarget=true;gltex->lost=false;
	configTexture(gltex,w,h,NULL,false);
	pTarget->w=w;pTarget->h=h;
	ok=buildTarget(pTarget,gltex->name,w,h,ms);
	if(!ok)
	{
		smLog("%s:" SLINE ": Failed to create render target.\n",GFX_GLFW_SRCFN);
		smTextureFree(pTarget->tex);
		delete pTarget;
		return 0;
	}
	pTarget->next=targets;
	targets=pTarget;
	return (SMTRG)pTarget;
}
SMTEX SMELT_IMPL::smTargetTexture(SMTRG targ)
{
	TRenderTargetList *pTarg=(TRenderTargetList*)targ;
	return targ?pTarg->tex:0;
}
void SMELT_IMPL::smTargetFree(SMTRG targ)
{
	TRenderTargetList *pTarget=targets,*pLastTarg=NULL;
	while(pTarget)
	{
		if((TRenderTargetList*)targ==pTarget)
		{
			if(pLastTarg)pLastTarg->next=pTarget->next;
			else targets=pTarget->next;
			if(curTarget==(TRenderTargetList*)targ)
			glBindFramebuffer(GL_FRAMEBUFFER_EXT,0);
			if(pTarget->depth)
			glDeleteRenderbuffers(1,&pTarget->depth);
			glDeleteFramebuffers(1,&pTarget->frame);
			if(pTarget->ms)
			{
				glDeleteRenderbuffers(1,&pTarget->colorms);
				glDeleteRenderbuffers(1,&pTarget->scolor);
				glDeleteRenderbuffers(1,&pTarget->sdepth);
				glDeleteFramebuffers(1,&pTarget->sframe);
			}
			if(curTarget==(TRenderTargetList*)targ)curTarget=0;
			smTextureFree(pTarget->tex);
			delete pTarget;
			return;
		}
		pLastTarg=pTarget;
		pTarget=pTarget->next;
	}
}
SMTEX SMELT_IMPL::smTextureCreate(int w,int h)
{
	DWORD *px=new DWORD[w*h];
	memset(px,0,sizeof(DWORD)*w*h);
	SMTEX ret=buildTexture(w,h,px);
	if(ret)
	{
		TTextureList *tex=new TTextureList;
		tex->tex=ret;
		tex->w=w;
		tex->h=h;
		tex->next=textures;
		textures=tex;
	}
	return ret;
}
SMTEX SMELT_IMPL::smTextureLoad(const char *path)
{
	FILE *pFile;DWORD size,rsize;char *buff;
	SMTEX ret=0;
	pFile=fopen(path,"rb");
	if(!pFile)return 0;
	fseek(pFile,0,SEEK_END);size=ftell(pFile);rewind(pFile);
	buff=(char*)malloc(sizeof(char)*size);
	if(!buff)return 0;
	rsize=fread(buff,1,size,pFile);
	if(rsize!=size)return 0;
	ret=smTextureLoadFromMemory(buff,size);if(!ret)return 0;
	glTexture *t=(glTexture*)ret;
	configTexture(t,t->rw,t->rh,t->px);
	delete[] t->px;t->px=NULL;
	t->fn=strcpy(new char[strlen(path)+1],path);
	free(buff);
	fclose(pFile);
	return ret;
}
SMTEX SMELT_IMPL::smTextureLoadFromMemory(const char *ptr,DWORD size)
{
	int w=0,h=0;SMTEX ret=0;
	DWORD *px=decodeImage((BYTE*)ptr,NULL,size,w,h);
	if(px)ret=buildTexture(w,h,px);
	if(ret)
	{
		TTextureList *tex=new TTextureList;
		tex->tex=ret;tex->w=w;tex->h=h;
		tex->next=textures;textures=tex;
	}else smLog("%s:" SLINE ": Unsupported texture format.\n",GFX_GLFW_SRCFN);
	return ret;
}
void SMELT_IMPL::smTextureFree(SMTEX tex)
{
	if(!pOpenGLDevice)return;
	TTextureList *ctex=textures,*lasttex=NULL;
	while(ctex)
	{
		if(ctex->tex==tex)
		{
			if(lasttex)lasttex->next=ctex->next;
			else textures=ctex->next;
			delete ctex;
			break;
		}
		lasttex=ctex;
		ctex=ctex->next;
	}
	if(tex)
	{
		glTexture *ptex=(glTexture*)tex;
		delete[] ptex->fn;
		delete[] ptex->locpx;
		delete[] ptex->px;
		glDeleteTextures(1,&ptex->name);
		delete ptex;
	}
}
void SMELT_IMPL::smTextureOpt(int potopt,int filter)
{
	batchOGL();
	if(potopt==TPOT_NONPOT)
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
	}
	if(potopt==TPOT_POT)
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_R,GL_REPEAT);
	}
	filtermode=filter;
	if(filter==TFLT_NEAREST)
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	}
	if(filter==TFLT_LINEAR)
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	}
}
int SMELT_IMPL::smTextureGetWidth(SMTEX tex,bool original)
{
	if(original)
	{
		TTextureList *ctex=textures;
		while(ctex){if(ctex->tex==tex)return ctex->w;ctex=ctex->next;}
	}
	else return ((glTexture*)tex)->rw;
	return 0;
}
int SMELT_IMPL::smTextureGetHeight(SMTEX tex,bool original)
{
	if(original)
	{
		TTextureList *ctex=textures;
		while(ctex){if(ctex->tex==tex)return ctex->h;ctex=ctex->next;}
	}
	else return ((glTexture*)tex)->rh;
	return 0;
}
DWORD* SMELT_IMPL::smTextureLock(SMTEX tex,int l,int t,int w,int h,bool ro)
{
	glTexture *ptex=(glTexture*)tex;
	if(ptex->locpx)return NULL;
	bool fromfile=(ptex->px==NULL&&ptex->fn);
	if(fromfile)
	{
		FILE *pFile;DWORD size,rsize;char *buff;
		pFile=fopen(ptex->fn,"rb");
		if(!pFile)return 0;
		fseek(pFile,0,SEEK_END);size=ftell(pFile);rewind(pFile);
		buff=(char*)malloc(sizeof(char)*size);
		if(!buff)return 0;
		rsize=fread(buff,1,size,pFile);
		if(rsize!=size)return 0;
		int _w,_h;
		ptex->px=decodeImage((BYTE*)buff,ptex->fn,size,_w,_h);
		if(_w!=(int)ptex->rw||_h!=(int)ptex->rh)
		{delete[] ptex->px;ptex->px=NULL;}
		free(buff);
		fclose(pFile);
		if(ptex->px&&!ro){delete[] ptex->fn;ptex->fn=NULL;}
	}
	if(!ptex->px&&!ptex->isTarget)return 0;
	if(!w)w=ptex->rw;if(!h)h=ptex->rh;
	//asserts...
	ptex->roloc=ro;ptex->locx=l;ptex->locy=t;ptex->locw=w;ptex->loch=h;
	ptex->locpx=new DWORD[w*h];
	if(!ptex->isTarget)
	{
		DWORD *dst=ptex->locpx,*src=ptex->px+((t*ptex->rw)+l);
		for(int i=0;i<h;++i)//TODO: flip it...
		{
			memcpy(dst,src,w*sizeof(DWORD));
			dst+=w;src+=ptex->rw;
		}
	}
	else return 0;
	return ptex->locpx;
}
void SMELT_IMPL::smTexutreUnlock(SMTEX tex)
{
	glTexture *ptex=(glTexture*)tex;
	if(!ptex->locpx)return;
	if(!ptex->roloc)
	{
		DWORD *src=ptex->locpx+ptex->locw*(ptex->loch-1),
			*dst=ptex->px+(((ptex->rh-ptex->locy-1)*ptex->rw)+ptex->locx);
		for(int i=0;i<ptex->loch;++i)
		{
			memcpy(dst,src,ptex->locw*sizeof(DWORD));
			dst-=ptex->rw;src-=ptex->locw;
		}
		if(ptex->lost)configTexture(ptex,ptex->rw,ptex->rh,ptex->px);
		else
		{
			glBindTexture(GL_TEXTURE_2D,ptex->name);
			glTexSubImage2D(GL_TEXTURE_2D,0,ptex->locx,
							(ptex->rh-ptex->locy)-ptex->loch,ptex->locw,ptex->loch,GL_RGBA,
							GL_UNSIGNED_BYTE,ptex->locpx);
			glBindTexture(GL_TEXTURE_2D,primTex?(((glTexture*)primTex)->name):0);
		}
	}
	if(ptex->fn&&ptex->roloc){delete[] ptex->px;ptex->px=NULL;}
	delete[] ptex->locpx;ptex->locpx=NULL;
	ptex->roloc=false;
	ptex->locx=ptex->locy=ptex->locw=ptex->loch=-1;
}

inline bool ispot(GLuint x){return((x&(x-1))==0);}
inline GLuint npot(GLuint x)
{
	--x;
	for(unsigned i=1;i<sizeof(GLuint)*8;i<<=1)x|=x>>i;
	return x+1;
}
DWORD* SMELT_IMPL::decodeImage(BYTE *data,const char *fn,DWORD size,int &w,int &h)
{
	w=h=0;
	DWORD *px=NULL;
	int fnlen=fn?strlen(fn):0;
	if((fnlen>5)&&(strcasecmp((fn+fnlen)-5,".rgba"))==0)//raw image... pending remove
	{
		DWORD *ptr=(DWORD*)data;
		DWORD _w=ptr[0],_h=ptr[1];
		if(((_w*_h*4)+8)==size)
		{
			w=_w;h=_h;
			px=new DWORD[w*h];
			memcpy(px,ptr+2,w*h*4);
		}
		return px;
	}
	CxImage img;
	img.Decode(data,size,CXIMAGE_FORMAT_UNKNOWN);
	if(img.IsValid())
	{
		w=img.GetWidth();h=img.GetHeight();
		px=new DWORD[w*h];
		BYTE *sptr=(BYTE*)px;
		bool achannel=img.AlphaIsValid();
		for(int i=0;i<h;++i)
		for(int j=0;j<w;++j)
		{
			RGBQUAD rgb=img.GetPixelColor(j,i,true);
			*(sptr++)=rgb.rgbRed;
			*(sptr++)=rgb.rgbGreen;
			*(sptr++)=rgb.rgbBlue;
			*(sptr++)=achannel?rgb.rgbReserved:0xFF;
		}
	}
	return px;
}
void SMELT_IMPL::bindTexture(glTexture *t)
{
	if(t&&t->lost)
	configTexture(t,t->rw,t->rh,t->px);
	if(((SMTEX)t)!=primTex)
	{
		glBindTexture(GL_TEXTURE_2D,t?t->name:((glTexture*)emptyTex)->name);
		glUniform1i(loc_tex,0);
		primTex=(SMTEX)t;
	}
}
bool SMELT_IMPL::buildTarget(TRenderTargetList *pTarget,GLuint texid,int w,int h,int ms=0)
{
	bool ok=true;
	if(ms)
	{
		glGenFramebuffers(1,&pTarget->sframe);
		glBindFramebuffer(GL_FRAMEBUFFER,pTarget->sframe);
		glGenRenderbuffers(1,&pTarget->scolor);
		glBindRenderbuffer(GL_RENDERBUFFER,pTarget->scolor);
		glRenderbufferStorage(GL_RENDERBUFFER,GL_RGBA,w,h);
		glGenRenderbuffers(1,&pTarget->sdepth);
		glBindRenderbuffer(GL_RENDERBUFFER,pTarget->sdepth);
		glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,w,h);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,pTarget->scolor);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,pTarget->sdepth);

		glGenFramebuffers(1,&pTarget->frame);
		glBindFramebuffer(GL_FRAMEBUFFER,pTarget->frame);
		glGenRenderbuffers(1,&pTarget->colorms);
		glBindRenderbuffer(GL_RENDERBUFFER,pTarget->colorms);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER,ms,GL_RGBA,w,h);
		glGenRenderbuffers(1,&pTarget->depth);
		glBindRenderbuffer(GL_RENDERBUFFER,pTarget->depth);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER,ms,GL_DEPTH_COMPONENT24,w,h);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,pTarget->colorms);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,pTarget->depth);
		GLenum rc=glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if((rc==GL_FRAMEBUFFER_COMPLETE)&&(glGetError()==GL_NO_ERROR))
		{
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			ok=true;pTarget->ms=ms;
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER,0);
			glDeleteRenderbuffers(1,&pTarget->colorms);
			glDeleteRenderbuffers(1,&pTarget->depth);
			glDeleteFramebuffers(1,&pTarget->frame);
			glDeleteFramebuffers(1,&pTarget->sframe);
			ok=false;ms=0;
		}
		glBindFramebuffer(GL_FRAMEBUFFER,curTarget?curTarget->frame:0);
	}
	if(!ms)
	{
		glGenFramebuffers(1,&pTarget->frame);
		glGenRenderbuffers(1,&pTarget->depth);
		glBindFramebuffer(GL_FRAMEBUFFER,pTarget->frame);
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,texid,0);
		glBindRenderbuffer(GL_RENDERBUFFER,pTarget->depth);
		glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,w,h);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,pTarget->depth);
		GLenum rc=glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if((rc==GL_FRAMEBUFFER_COMPLETE)&&(glGetError()==GL_NO_ERROR))
		{
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			ok=true;pTarget->ms=0;
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER,0);
			glDeleteRenderbuffers(1,&pTarget->depth);
			glDeleteFramebuffers(1,&pTarget->frame);
			ok=false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER,curTarget?curTarget->frame:0);
	}
	return ok;
}
void SMELT_IMPL::configTexture(glTexture *t,int w,int h,DWORD *px,bool compressed)
{
	GLuint tex=0;
	glGenTextures(1,&tex);
	t->lost=false;t->name=tex;t->rw=w;t->rh=h;t->px=px;
	t->dw=t->dh=0;
	bool fromfile=(!px&&(t->fn));
	if(fromfile)
	{
		FILE *pFile;DWORD size,rsize;char *buff;
		pFile=fopen(t->fn,"rb");
		if(!pFile)return;
		fseek(pFile,0,SEEK_END);size=ftell(pFile);rewind(pFile);
		buff=(char*)malloc(sizeof(char)*size);
		if(!buff)return;
		rsize=fread(buff,1,size,pFile);
		if(rsize!=size)return;
		int _w,_h;
		px=decodeImage((BYTE*)buff,t->fn,size,_w,_h);
		if(_w!=w||_h!=h){delete[] px;px=NULL;}
		free(buff);
		fclose(pFile);
	}
	glBindTexture(GL_TEXTURE_2D,tex);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_LOD,0.0f);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAX_LOD,0.0f);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_BASE_LEVEL,0);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL,0);
	compressed=false;//compression is unusable at this moment
	const GLenum fmt=compressed?/*GL_COMPRESSED_RGBA_S3TC_DXT5_EXT*/GL_COMPRESSED_RGBA_ARB:GL_RGBA8;
	glTexImage2D(GL_TEXTURE_2D,0,fmt,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,px);
	glBindTexture(GL_TEXTURE_2D,primTex?(((glTexture*)primTex)->name):0);
	if(fromfile)delete[] px;
}
SMTEX SMELT_IMPL::buildTexture(int w,int h,DWORD *px)
{
	glTexture *ret=new glTexture;
	memset(ret,0,sizeof(glTexture));
	ret->lost=true;
	ret->rw=w;ret->rh=h;ret->px=px;
	return (SMTEX)ret;
}
void SMELT_IMPL::configProjectionMatrix2D(int w,int h)
{
	memset(mproj,0,sizeof(mproj));
	mproj[0]=2./w;
	mproj[5]=2./h;
	mproj[10]=-2.;
	mproj[15]=1.;
	mproj[12]=mproj[13]=mproj[14]=-1.;
	glUniformMatrix4fv(loc_mproj,1,0,mproj);
}
void SMELT_IMPL::configProjectionMatrix3D(int w,int h,float fov)
{
	memset(mproj,0,sizeof(mproj));
	float f=1./tanf(M_PI*fov/360.);
	float ar=(float)w/(float)h;
	float Near=0.1,Far=1000.;
	mproj[0]=f/ar;mproj[5]=f;
	mproj[10]=(Far+Near)/(Near-Far);mproj[11]=-1.0f;
	mproj[14]=(2*Far*Near)/(Near-Far);
	glUniformMatrix4fv(loc_mproj,1,0,mproj);
}
void SMELT_IMPL::batchOGL(bool endScene)
{
	if(vertexArray&&primcnt)
	{
		float twm=-1.,thm=-1.;
		if(primTex)
		{
			glTexture *ptex=(glTexture*)primTex;
			if(ptex->dw&&ptex->dh)
			{
				twm=(ptex->rw)/(float)(ptex->dw);
				thm=(ptex->rh)/(float)(ptex->dh);
			}
		}
		if(!tdmode)
		{
			float h=curTarget?curTarget->h:scrh;
			for(int i=0;i<primcnt*primType;++i)
			{
				vertexArray[i].y=h-vertexArray[i].y;
				vertexArray[i].z=-vertexArray[i].z;
			}
		}
		if(twm>0&&thm>0)
		for(int i=0;i<primcnt*primType;++i)
		{
			vertexArray[i].tx*=twm;
			vertexArray[i].ty*=thm;
		}
		glBindVertexArray(VertexArrayObject);
		glBindBuffer(GL_ARRAY_BUFFER,VertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER,sizeof(smVertex)*primcnt*primType,vertexBuf,GL_DYNAMIC_DRAW);
		switch(primType)
		{
			case PRIM_LINES:
				glDrawArrays(GL_LINES,0,2*primcnt);
			break;
			case PRIM_TRIANGLES:
				glDrawArrays(GL_TRIANGLES,0,3*primcnt);
			break;
			case PRIM_QUADS:
				glDrawElements(GL_TRIANGLES,6*primcnt,GL_UNSIGNED_SHORT,0);
#if 0
				for (int i=0;i<primcnt*6;i+=3)
				{
					printf("idxbuf:%d\n",indexBuf[i]);
					printf("QUAD'S TRIANGLE:\n");
#define printVertex(a) printf("(%.2f,%.2f,%.2f),0x%X,(%.2f,%.2f)\n",a.x,a.y,a.z,a.col,a.tx,a.ty);
					printf("#%d: ",indexBuf[i+0]);printVertex(vertexBuf[indexBuf[i+0]]);
					printf("#%d: ",indexBuf[i+1]);printVertex(vertexBuf[indexBuf[i+1]]);
					printf("#%d: ",indexBuf[i+2]);printVertex(vertexBuf[indexBuf[i+2]]);
#undef printVertex
				}
#endif
			break;
		}
		glBindVertexArray(0);
		primcnt=0;
	}
	if(vertexArray)vertexArray=endScene?0:vertexBuf;
}
void SMELT_IMPL::setBlend(int blend)
{
	if((blend&BLEND_ALPHABLEND)!=(primBlend&BLEND_ALPHABLEND))
	{
		if(blend&BLEND_ALPHABLEND)glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		else glBlendFunc(blend&BLEND_COLORINV?GL_ONE_MINUS_DST_COLOR:GL_SRC_ALPHA,blend&BLEND_COLORINV?GL_ZERO:GL_ONE);
	}
	if((blend&BLEND_ZWRITE)!=(primBlend&BLEND_ZWRITE))
	{
		if(blend&BLEND_ZWRITE||tdmode)glDepthMask(GL_TRUE);
		else glDepthMask(GL_FALSE);
	}
	if((blend&BLEND_COLORADD)!=(primBlend&BLEND_COLORADD))
	{
		if(blend&BLEND_COLORADD)glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_ADD);
		else glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	}
	if((blend&BLEND_COLORINV)!=(primBlend&BLEND_COLORINV))
	{
		if(blend&BLEND_COLORINV)glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
		else glBlendFunc(GL_SRC_ALPHA,blend&BLEND_ALPHABLEND?GL_ONE_MINUS_SRC_ALPHA:GL_ONE);
	}
	primBlend=blend;
}
void SMELT_IMPL::unloadGLEntryPoints()
{
}
bool SMELT_IMPL::checkGLExtension(const char *extlist,const char *ext)
{
	return true;/*shit*
	const char *ptr=strstr(extlist,ext);
	if(ptr==NULL)return false;
	const char endchar=ptr[strlen(ext)];
	if((endchar=='\0')||(endchar==' '))return true;
	return false;*/
}
bool SMELT_IMPL::loadGLEntryPoints()
{
	smLog("%s:" SLINE ": Initializing with OpenGL core profile...\n",GFX_GLFW_SRCFN);
	pOpenGLDevice->have_base_opengl=true;
	/*
	 * All OpenGL features utilized by SMELT are in the core profile
	 * for OpenGL >= 3.2. So this function is essentially useless.
	 */
	glewExperimental=true;
	GLenum glewret=glewInit();
	if(glewret)
	{
		smLog("%s:" SLINE ": glewInit() failed with error %s\n",GFX_GLFW_SRCFN,glewGetErrorString(glewret));
		pOpenGLDevice->have_base_opengl=false;
		return false;
	}
	if (!pOpenGLDevice->have_base_opengl)
	{
		unloadGLEntryPoints();
		return false;
	}
	smLog("%s:" SLINE ": GL_RENDERER: %s\n",GFX_GLFW_SRCFN,(const char *)glGetString(GL_RENDERER));
	smLog("%s:" SLINE ": GL_VENDOR: %s\n",GFX_GLFW_SRCFN,(const char *)glGetString(GL_VENDOR));
	smLog("%s:" SLINE ": GL_VERSION: %s\n",GFX_GLFW_SRCFN,(const char *)glGetString(GL_VERSION));
	const char *verstr=(const char*)glGetString(GL_VERSION);
	int maj=0,min=0;
	sscanf(verstr,"%d.%d",&maj,&min);
	if((maj<3)||((maj==3)&&(min<2)))
	{
		smLog("%s:" SLINE ": OpenGL implementation must be at least version 3.2.\n",GFX_GLFW_SRCFN);
		unloadGLEntryPoints();
		return false;
	}
	return true;
}
bool SMELT_IMPL::initOGL()
{
	primTex=0;emptyTex=0;
	if(pOpenGLDevice){smLog("%s:" SLINE ": Multiple initialization!\n",GFX_GLFW_SRCFN);return false;}
	pOpenGLDevice=new TOpenGLDevice;
	if(!loadGLEntryPoints())return false;
	smLog("%s:" SLINE ": Mode: %d x %d\n",GFX_GLFW_SRCFN,scrw,scrh);
	vertexArray=NULL;textures=NULL;
	ShaderProgram=vertshader=fragshader=0;
	VertexBufferObject=VertexArrayObject=IndexBufferObject=0;
	if(!confOGL())return false;
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	if(!curTarget)glfwSwapBuffers((GLFWwindow*)hwnd);
	return true;
}
void SMELT_IMPL::finiOGL()
{
	while(textures)smTextureFree(textures->tex);
	while(targets)smTargetFree((SMTRG)targets);
	textures=NULL;targets=NULL;vertexArray=NULL;
	delete[] vertexBuf;vertexBuf=NULL;
	delete[] indexBuf;indexBuf=NULL;
	if(pOpenGLDevice)
	{
		if(VertexBufferObject!=0)
		{
			glBindVertexArray(VertexArrayObject);
			glBindBuffer(GL_ARRAY_BUFFER,0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
			glDeleteBuffers(1,&VertexBufferObject);
			glDeleteBuffers(1,&IndexBufferObject);
			glBindVertexArray(0);
			glDeleteVertexArrays(1,&VertexArrayObject);
			VertexArrayObject=0;
			VertexBufferObject=0;
			IndexBufferObject=0;
		}
		glDeleteProgram(ShaderProgram);
		delete pOpenGLDevice;
		pOpenGLDevice=NULL;
	}
}
bool SMELT_IMPL::restOGL()
{
	if(!pOpenGLDevice)return false;
	delete[] vertexBuf;vertexBuf=NULL;
	delete[] indexBuf;indexBuf=NULL;
	unloadGLEntryPoints();
	if(!loadGLEntryPoints())return false;
	if(!confOGL())return false;
	//if()return();//TODO: rest func
	return true;
}
bool SMELT_IMPL::confOGL()
{
	bindTexture(NULL);
	DWORD ones=~0U;
	if(!emptyTex)
	{emptyTex=buildTexture(1,1,&ones);}
	configTexture((glTexture*)emptyTex,1,1,&ones,false);
	for(TTextureList *i=textures;i;i=i->next)
	{
		glTexture *t=(glTexture*)i->tex;
		if(!t)continue;t->lost=true;t->name=0;
	}
	TRenderTargetList *target=targets;
	while(target)
	{
		glTexture *tex=(glTexture*)target->tex;
		bindTexture(tex);bindTexture(NULL);
		buildTarget(target,tex?tex->name:0,target->w,target->h);
		target=target->next;
	}
	int compret=0;
	ShaderProgram=glCreateProgram();
	vertshader=glCreateShader(GL_VERTEX_SHADER);
	fragshader=glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertshader,1,&fixedfunc_pipeline_vsh,NULL);
	glShaderSource(fragshader,1,&fixedfunc_pipeline_fsh,NULL);
	glCompileShader(vertshader);
	glGetShaderiv(vertshader,GL_COMPILE_STATUS,&compret);
	//char log[1024];
	//glGetShaderInfoLog(vertshader,1024,NULL,log);
	//puts(log);
	if(!compret)
	smLog("%s:" SLINE ": Warning: Your shitty vertex shader failed to compile!\n",GFX_GLFW_SRCFN);
	glCompileShader(fragshader);
	glGetShaderiv(fragshader,GL_COMPILE_STATUS,&compret);
	//glGetShaderInfoLog(fragshader,1024,NULL,log);
	//puts(log);
	if(!compret)
	smLog("%s:" SLINE ": Warning: Your shitty fragment shader failed to compile!\n",GFX_GLFW_SRCFN);
	glAttachShader(ShaderProgram,vertshader);
	glAttachShader(ShaderProgram,fragshader);
	glLinkProgram(ShaderProgram);
	//glGetProgramInfoLog(ShaderProgram,1024,NULL,log);
	//puts(log);
	glGetProgramiv(ShaderProgram,GL_LINK_STATUS,&compret);
	if(!compret)
	smLog("%s:" SLINE ": Warning: Default shader linkage failure!\n",GFX_GLFW_SRCFN);
	glDeleteShader(vertshader);
	glDeleteShader(fragshader);
	glUseProgram(ShaderProgram);
	loc_tex=glGetUniformLocation(ShaderProgram,"tex");
	loc_mmodv=glGetUniformLocation(ShaderProgram,"mmodv");
	loc_mproj=glGetUniformLocation(ShaderProgram,"mproj");
	vertexBuf=new smVertex[VERTEX_BUFFER_SIZE];
	indexBuf=new GLushort[VERTEX_BUFFER_SIZE*6/4];
	GLushort* indices=indexBuf;
	for(int i=0,n=0;i<VERTEX_BUFFER_SIZE/4;++i)
	{
		*indices++=n;*indices++=n+1;
		*indices++=n+2;*indices++=n+2;
		*indices++=n+3;*indices++=n;
		n+=4;
	}
	glGenBuffers(1,&VertexBufferObject);
	glGenVertexArrays(1,&VertexArrayObject);
	glGenBuffers(1,&IndexBufferObject);
	glBindVertexArray(VertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER,VertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER,sizeof(smVertex)*VERTEX_BUFFER_SIZE,vertexBuf,GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,IndexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLushort)*((VERTEX_BUFFER_SIZE*6)/4),indexBuf,GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT,0,sizeof(smVertex),(void*)offsetof(smVertex,x));//vp
	glVertexAttribPointer(1,4,GL_UNSIGNED_BYTE,1,sizeof(smVertex),(void*)offsetof(smVertex,col));//vc
	glVertexAttribPointer(2,2,GL_FLOAT,0,sizeof(smVertex),(void*)offsetof(smVertex,tx));//vtc
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glPixelStorei(GL_PACK_ALIGNMENT,1);
	glEnable(GL_SCISSOR_TEST);
	glDisable(GL_CULL_FACE);
	glActiveTexture(GL_TEXTURE0);
	glDepthFunc(GL_GEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	filtermode=TFLT_LINEAR;
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	//GL_REPEAT doesn't work with non-pot textures...
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
	primcnt=0;primType=PRIM_QUADS;primBlend=BLEND_ALPHABLEND;primTex=0;
	glScissor(0,0,scrw,scrh);
	glViewport(0,0,scrw,scrh);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	configProjectionMatrix2D(scrw,scrh);
	_smMatrix Mmodv;Mmodv.loadIdentity();
	memcpy(mmodv,Mmodv.m,sizeof(mmodv));
	glUniformMatrix4fv(loc_mmodv,1,0,mmodv);
	return true;
}
#else
#include "gfx_glfw_compat.cpp"
#endif //USE_OPENGL_COMPATIBILITY_PROFILE
