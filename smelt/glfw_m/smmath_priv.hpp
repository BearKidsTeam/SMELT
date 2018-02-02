// -*- C++ -*-
/*
 * Simple MultimEdia LiTerator(SMELT)
 * by Chris Xiong 2015
 * Math header & implementation for the core
 *
 * WARNING: This library is in development and interfaces would be very
 * unstable.
 *
 */
#ifndef SMMATH_H
#define SMMATH_H
#include <cmath>
#include <cstddef>
#define sqr(x) ((x)*(x))
#define EPS 1e-6
#ifndef PI
#define PI 3.14159265358979323846f
#endif

class smMath
{
public:
	static double deg2rad(double deg){return deg/180.*PI;}
	static double rad2deg(double rad){return rad/PI*180.;}
	static double clamprad(double a){while(a<0)a+=2*PI;while(a>2*PI)a-=2*PI;return a;}
	static double clampdeg(double a){while(a<0)a+=360.;while(a>360)a-=360.;return a;}
};
class smvec2d
{
public:
	float x,y;
	smvec2d(float _x,float _y){x=_x;y=_y;}
	smvec2d(){x=y=.0;}
	float l(){return sqrtf(sqr(x)+sqr(y));}
	void normalize(){float L=l();if(L<EPS)return;x/=L;y/=L;}
	smvec2d getNormalized(){float L=l();if(L<EPS)return smvec2d(0,0);return smvec2d(x/L,y/L);}
	void swap(){float t=x;x=y;y=t;}
	void rotate(float rad){float tx=x*cosf(rad)+y*sinf(rad),ty=y*cosf(rad)-x*sinf(rad);x=tx,y=ty;}
	smvec2d getRotate(float rad){float tx=x*cosf(rad)+y*sinf(rad),ty=y*cosf(rad)-x*sinf(rad);return smvec2d(tx,ty);}
	friend smvec2d operator -(smvec2d a,smvec2d b){return smvec2d(a.x-b.x,a.y-b.y);}
	friend smvec2d operator +(smvec2d a,smvec2d b){return smvec2d(a.x+b.x,a.y+b.y);}
	friend float operator |(smvec2d a,smvec2d b){return a.x*b.x+a.y*b.y;}
	friend float operator *(smvec2d a,smvec2d b){return a.x*b.y-b.x*a.y;}
	friend smvec2d operator *(float a,smvec2d b){return smvec2d(a*b.x,a*b.y);}
	friend smvec2d operator *(smvec2d a,float b){return smvec2d(b*a.x,b*a.y);}
	friend float operator ^(smvec2d a,smvec2d b){return (a|b)/a.l()/b.l();}
};

class smvec3d
{
public:
	float x,y,z;
	smvec3d(float _x,float _y,float _z){x=_x;y=_y;z=_z;}
	smvec3d(smvec2d a,float _z=.0){x=a.x;y=a.y;z=_z;}
	smvec3d(){x=y=z=.0;}
	float l(){return sqrtf(sqr(x)+sqr(y)+sqr(z));}
	void normalize(){float L=l();if(L<EPS)return;x/=L;y/=L;z/=L;}
	smvec3d getNormalized(){float L=l();if(L<EPS)return smvec3d(0,0,0);return smvec3d(x/L,y/L,z/L);}
	friend smvec3d operator -(smvec3d a,smvec3d b){return smvec3d(a.x-b.x,a.y-b.y,a.z-b.z);}
	friend smvec3d operator +(smvec3d a,smvec3d b){return smvec3d(a.x+b.x,a.y+b.y,a.z+b.z);}
	friend float operator |(smvec3d a,smvec3d b){return a.x*b.x+a.y*b.y+a.z*b.z;}
	friend smvec3d operator *(smvec3d a,smvec3d b){return smvec3d(a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x);}
	friend smvec3d operator *(float a,smvec3d b){return smvec3d(a*b.x,a*b.y,a*b.z);}
	friend smvec3d operator *(smvec3d a,float b){return smvec3d(b*a.x,b*a.y,b*a.z);}
	friend float operator ^(smvec3d a,smvec3d b){return (a|b)/a.l()/b.l();}
};

class smvec4d
{
public:
	float x,y,z,w;
	smvec4d(float _x,float _y,float _z,float _w){x=_x;y=_y;z=_z;w=_w;}
	smvec4d(smvec3d a,float _w=.0){x=a.x;y=a.y;z=a.z;w=_w;}
	smvec4d(){x=y=z=w=.0;}
	float l(){return sqrt(sqr(x)+sqr(y)+sqr(z)+sqr(w));}
	void normalize(){float L=l();if(L<EPS)return;x/=L;y/=L;z/=L;w/=L;}
	smvec4d getNormalized(){float L=l();if(L<EPS)return smvec4d(0,0,0,0);return smvec4d(x/L,y/L,z/L,w/L);}
	friend smvec4d operator -(smvec4d a,smvec4d b){return smvec4d(a.x-b.x,a.y-b.y,a.z-b.z,a.w-b.w);}
	friend smvec4d operator +(smvec4d a,smvec4d b){return smvec4d(a.x+b.x,a.y+b.y,a.z+b.z,a.w+b.w);}
	friend float operator |(smvec4d a,smvec4d b){return a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w;}
	//Note: this doesn't do a real 4D cross product.
	friend smvec4d operator *(smvec4d a,smvec4d b){return smvec4d(a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x,1);}
	friend smvec4d operator *(float a,smvec4d b){return smvec4d(a*b.x,a*b.y,a*b.z,a*b.w);}
	friend smvec4d operator *(smvec4d a,float b){return smvec4d(b*a.x,b*a.y,b*a.z,b*a.w);}
	friend float operator ^(smvec4d a,smvec4d b){return (a|b)/a.l()/b.l();}
};

class smMatrix
{
public:
	float m[16];
	/* sf  0  1  2  3
	 * 0  00 04 08 12
	 * 1  01 05 09 13
	 * 2  02 06 10 14
	 * 3  03 07 11 15
	 */
	smMatrix(){for(int i=0;i<16;++i)m[i]=.0;}
	smMatrix(const float* _m){memcpy(m,_m,sizeof(m));}
	smMatrix(const smMatrix &copy){for(int i=0;i<16;++i)m[i]=copy.m[i];}
	float* operator [](int s){if(s>=0&&s<4)return m+s*4;else return NULL;}
	void clear(){for(int i=0;i<16;++i)m[i]=.0;}
	void loadIdentity(){clear();m[0]=m[5]=m[10]=m[15]=1.;}
	void translate(float x,float y,float z)
	{
		smMatrix tmp;tmp.loadIdentity();
		tmp.m[12]=x;tmp.m[13]=y;tmp.m[14]=z;
		*this=*this*tmp;
	}
	void rotate(float a,float x,float y,float z)
	{
		if(smvec3d(x,y,z).l()<=EPS)return;
		if(fabsf(smvec3d(x,y,z).l()-1)>EPS)
		{
			smvec3d a(x,y,z);a.normalize();
			x=a.x;y=a.y;z=a.z;
		}
		smMatrix tmp;
		float c=cosf(a),s=sinf(a);
		tmp.m[ 0]=x*x*(1-c)+c;
		tmp.m[ 4]=x*y*(1-c)-z*s;
		tmp.m[ 8]=x*z*(1-c)+y*s;
		tmp.m[ 1]=y*x*(1-c)+z*s;
		tmp.m[ 5]=y*y*(1-c)+c;
		tmp.m[ 9]=y*z*(1-c)-x*s;
		tmp.m[ 2]=x*z*(1-c)-y*s;
		tmp.m[ 6]=y*z*(1-c)+x*s;
		tmp.m[10]=z*z*(1-c)+c;
		tmp.m[15]=1;
		*this=*this*tmp;
	}
	void lookat(smvec3d eye,smvec3d at,smvec3d up)
	{
		smvec3d f=at-eye;f.normalize();up.normalize();
		smvec3d s=f*up;smvec3d u=s.getNormalized()*f;
		m[0]= s.x;m[4]= s.y;m[ 8]= s.z;m[12]=0;
		m[1]= u.x;m[5]= u.y;m[ 9]= u.z;m[13]=0;
		m[2]=-f.x;m[6]=-f.y;m[10]=-f.z;m[14]=0;
		m[3]=   0;m[7]=   0;m[11]=   0;m[15]=1;
	}
	friend smMatrix operator *(smMatrix a,smMatrix b)
	{
		smMatrix ret;
		for(int i=0;i<4;++i)
		for(int j=0;j<4;++j)
		for(int k=0;k<4;++k)
			ret[j][i]+=a[k][i]*b[j][k];
		return ret;
	}
	friend smvec3d operator *(smMatrix a,smvec3d b)
	{
		return smvec3d(a[0][0]*b.x+a[1][0]*b.y+a[2][0]*b.z,
		               a[0][1]*b.x+a[1][1]*b.y+a[2][1]*b.z,
		               a[0][2]*b.x+a[1][2]*b.y+a[2][2]*b.z);
	}
	friend smvec4d operator *(smMatrix a,smvec4d b)
	{
		return smvec4d(a[0][0]*b.x+a[1][0]*b.y+a[2][0]*b.z+a[3][0]*b.w,
		               a[0][1]*b.x+a[1][1]*b.y+a[2][1]*b.z+a[3][1]*b.w,
		               a[0][2]*b.x+a[1][2]*b.y+a[2][2]*b.z+a[3][2]*b.w,
		               a[0][3]*b.x+a[1][3]*b.y+a[2][3]*b.z+a[3][3]*b.w);
	}
};
#endif
