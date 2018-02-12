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
#define PI 3.14159265358979323846264338327950288f
#endif

class _smMath
{
public:
	static double deg2rad(double deg){return deg/180.*PI;}
	static double rad2deg(double rad){return rad/PI*180.;}
	static double clamprad(double a){while(a<0)a+=2*PI;while(a>2*PI)a-=2*PI;return a;}
	static double clampdeg(double a){while(a<0)a+=360.;while(a>360)a-=360.;return a;}
};

class _smvec3d
{
public:
	double x,y,z;
	_smvec3d(double _x,double _y,double _z){x=_x;y=_y;z=_z;}
	_smvec3d(){x=y=z=.0;}
	double l(){return sqrt(sqr(x)+sqr(y)+sqr(z));}
	void normalize(){double L=l();if(L<EPS)return;x/=L;y/=L;z/=L;}
	_smvec3d getNormalized(){double L=l();if(L<EPS)return _smvec3d(0,0,0);return _smvec3d(x/L,y/L,z/L);}
	friend _smvec3d operator -(_smvec3d a,_smvec3d b){return _smvec3d(a.x-b.x,a.y-b.y,a.z-b.z);}
	friend _smvec3d operator +(_smvec3d a,_smvec3d b){return _smvec3d(a.x+b.x,a.y+b.y,a.z+b.z);}
	friend double operator |(_smvec3d a,_smvec3d b){return a.x*b.x+a.y*b.y+a.z*b.z;}
	friend _smvec3d operator *(_smvec3d a,_smvec3d b){return _smvec3d(a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x);}
	friend _smvec3d operator *(double a,_smvec3d b){return _smvec3d(a*b.x,a*b.y,a*b.z);}
	friend _smvec3d operator *(_smvec3d a,double b){return _smvec3d(b*a.x,b*a.y,b*a.z);}
	friend double operator ^(_smvec3d a,_smvec3d b){return (a|b)/a.l()/b.l();}
};

class _smMatrix
{
public:
	float m[16];
	/* sf  0  1  2  3
	 * 0  00 04 08 12
	 * 1  01 05 09 13
	 * 2  02 06 10 14
	 * 3  03 07 11 15
	 */
	_smMatrix(){for(int i=0;i<16;++i)m[i]=.0;}
	_smMatrix(const float* _m){memcpy(m,_m,sizeof(m));}
	_smMatrix(const _smMatrix &copy){for(int i=0;i<16;++i)m[i]=copy.m[i];}
	float* operator [](int s){if(s>=0&&s<4)return m+s*4;else return NULL;}
	void clear(){for(int i=0;i<16;++i)m[i]=.0;}
	void loadIdentity(){clear();m[0]=m[5]=m[10]=m[15]=1.;}
	void translate(float x,float y,float z)
	{
		_smMatrix tmp;tmp.loadIdentity();
		tmp.m[12]=x;tmp.m[13]=y;tmp.m[14]=z;
		*this=*this*tmp;
	}
	void rotate(float a,float x,float y,float z)
	{
		if(_smvec3d(x,y,z).l()<=EPS)return;
		if(fabsf(_smvec3d(x,y,z).l()-1)>EPS)
		{
			_smvec3d a(x,y,z);a.normalize();
			x=a.x;y=a.y;z=a.z;
		}
		_smMatrix tmp;
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
	friend _smMatrix operator *(_smMatrix a,_smMatrix b)
	{
		_smMatrix ret;
		for(int i=0;i<4;++i)
		for(int j=0;j<4;++j)
		for(int k=0;k<4;++k)
			ret[j][i]+=a[k][i]*b[j][k];
		return ret;
	}
};
#endif
