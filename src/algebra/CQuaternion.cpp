/*	Quaternion class

	Last Modified	20/4/00
*/
#include <QtDebug>

#include <math.h>
#include <memory.h>
#include <assert.h>

#include "CQuaternion.h"

CQuaternion::CQuaternion()
{
	//identity();
}

CQuaternion::CQuaternion(const CQuaternion &q)
{
	this->x = q.getX();
	this->y = q.getY();
	this->z = q.getZ();
	this->w = q.getW();
}
////////

CQuaternion::CQuaternion(const float x, const float y, const float z, const float w)
{
	this->x = x;
	this->y = y;
	this->z = z;

	this->w = w;

   assert(!VISc::MathUtils::Equal(0.0, norm()));		// norm should never be close to 0

	normalize();
}	// end constructor

////////

CQuaternion::CQuaternion(const float x, const float y, const float z)
{
	eulerToQuat(x, y, z);
}	// end constructor

////////

CQuaternion::CQuaternion(CVector &v, const float angle)
{
	axisToQuat(v, angle);

	return;
}	// end constructor

////////

/*__forceinline*/inline double CQuaternion::norm(void)
{
   return(std::sqrt( std::pow(x,2.0f) + std::pow(y,2.0f) + std::pow(z,2.0f) + std::pow(w,2.0f)));
}	// end float Norm()

////////

void CQuaternion::identity(void)
{
	this->x	= 0.0f;
	this->y	= 0.0f;
	this->z	= 0.0f;
	this->w	= 1.0f;
}	// end void Identity()

///////

void CQuaternion::zeroClamp(void)
{
	x = VISc::MathUtils::ClampToZero(x);
	y = VISc::MathUtils::ClampToZero(y);
	z = VISc::MathUtils::ClampToZero(z);

	w = VISc::MathUtils::ClampToZero(w);

	return;
}	// end void Zero_Clamp()

////////

void CQuaternion::normalize(void)
{
	double norm_ = norm();

	assert(!VISc::MathUtils::Equal(0.0, norm_));		// norm should never be close to 0

	x = float(x / norm_);
	y = float(y / norm_);
	z = float(z / norm_);
	w = float(w / norm_);

	assert(VISc::MathUtils::Equal(1.0, norm()));		// must be normalized, safe

   VISc::MathUtils::Bound<float>(-1.0, w, 1.0);
	//LIMIT_RANGE(-1.0f, w, 1.0f);

	//LIMIT_RANGE(-1.0f, x, 1.0f);
   VISc::MathUtils::Bound<float>(-1.0, x, 1.0);
	//LIMIT_RANGE(-1.0f, y, 1.0f);
   VISc::MathUtils::Bound<float>(-1.0, y, 1.0);
	//LIMIT_RANGE(-1.0f, z, 1.0f);
   VISc::MathUtils::Bound<float>(-1.0, z, 1.0);

	return;
}	// end void Normalize()

////////

void CQuaternion::getValues(float &x, float &y, float &z, float &w) const
{
	x = this->x;
	y = this->y;
	z = this->z;

	w = this->w;

	return;
}	// end void GetValues

////////

void CQuaternion::axisToQuat(CVector &v, const float angle)
{
	float x,y,z;			// temp vars of vector
	double rad, scale;		// temp vars

	
	if ( v.isZero() )			// if axis is zero, then return quaternion (1,0,0,0)
	{
		w	= 1.0f;
		x	= 0.0f;
		y	= 0.0f;
		z	= 0.0f;

		return;
	}

	assert(v.isUnit());		// make sure the axis is a unit vector

	rad		= angle / 2;

	w		= (float)cos(rad);

	scale	= sin(rad);

	x = (float) v.getX();
	y = (float) v.getY();
	z = (float) v.getZ();
	//v.GetValues(x, y, z);

	this->x = float(x * scale);
	this->y = float(y * scale);
	this->z = float(z * scale);

	normalize();		// make sure a unit quaternion turns up

	return;
}	// end void AxisToQuat(..)

/////////

void CQuaternion::eulerToQuat(const float x, const float y, const float z)
{
	double	ex, ey, ez;		// temp half euler angles
	double	cr, cp, cy, sr, sp, sy, cpcy, spsy;		// temp vars in roll,VISc::MathUtils::PItch yaw

	ex = VISc::MathUtils::DegreesToRadians(x) / 2.0;	// convert to rads and half them
	ey = VISc::MathUtils::DegreesToRadians(y) / 2.0;
	ez = VISc::MathUtils::DegreesToRadians(z) / 2.0;

	cr = cos(ex);
	cp = cos(ey);
	cy = cos(ez);

	sr = sin(ex);
	sp = sin(ey);
	sy = sin(ez);

	cpcy = cp * cy;
	spsy = sp * sy;

	this->w = float(cr * cpcy + sr * spsy);

	this->x = float(sr * cpcy - cr * spsy);
	this->y = float(cr * sp * cy + sr * cp * sy);
	this->z = float(cr * cp * sy - sr * sp * cy);

	normalize();
	
	return;
}	// end void EulerToQuat(..)

////////

void CQuaternion::getAxisAngle(CVector &v, float &angle) const
{
	double	temp_angle;		// temp angle
	double	scale;			// temp vars

	temp_angle = acos(w);

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// Another version where scale is VISc::MathUtils::Powt (x2 + y2 + z2)
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   scale = (float)std::sqrt(VISc::MathUtils::Pow(x) + VISc::MathUtils::Pow(y) + VISc::MathUtils::Pow(z));
//	scale = (float)sin(temp_angle);

	assert(0 <= temp_angle);		// make sure angle is 0 - VISc::MathUtils::PI
	assert(VISc::MathUtils::PI >= temp_angle);

	if (VISc::MathUtils::Equal(0.0f, scale))		// angle is 0 or 360 so just simply set axis to 0,0,1 with angle 0
	{
		angle = 0.0f;

		v.set(0.0f, 0.0f, 1.0f);		// any axis will do
	}
	else
	{
		angle = (float)(temp_angle * 2.0);		// angle in radians

		v.set(float(x / scale), float(y / scale), float(z / scale));
		//v.Normalize();
		v.normalize();

		assert(0.0f <= angle);			// make sure rotation around axis is 0 - 360
		assert(2*VISc::MathUtils::PI >= angle);
		assert(v.isUnit());				// make sure a unit axis comes up
	}

	return;
}	// end void GetAxisAngle(..)

////////

void CQuaternion::getEulerAngles(float &x, float &y, float &z) const
{
	CMatrix matrix;			// temp matrix

	getMatrix(matrix);		// get matrix of this quaternion

	matrix.getEulerAngles(x, y, z);

	return;
}	// end void GetEulerAngles(.,)

////////

void CQuaternion::getMatrix(CMatrix &m) const
{
	double x2, y2, z2, w2, xy, xz, yz, wx, wy, wz;

	float matrix[16];		// temp matrix

	memset(matrix, 0, sizeof(float)*16);		// clear out matrix first
	matrix[15] = 1;								// last entry is 1

	x2	= VISc::MathUtils::Pow(x);	y2	= VISc::MathUtils::Pow(y);	z2	= VISc::MathUtils::Pow(z);	w2	= VISc::MathUtils::Pow(w);

	xy	= x * y;
	xz	= x * z;
	yz	= y * z;
	wx	= w * x;
	wy	= w * y;
	wz	= w * z;

/*	This is the old version
	// this gives the same matrix as uu^T + cos(angle)(I - uu^T) + sin(angle)S
	matrix[0] =	w2 + x2 -y2 - z2;
	matrix[1] =	2 * (xy + wz);
	matrix[2] =	2 * (xz - wy);

	matrix[4] =	2 * (xy - wz);
	matrix[5] =	w2 - x2 + y2 - z2;
	matrix[6] =	2 * (yz + wx);

	matrix[8] =	2 * (xz + wy);
	matrix[9] =	2 * (yz - wx);
	matrix[10] =	w2 - x2 - y2 + z2;
*/
//
	// should be slightly more accurate than above due to rearranging for unit quaternions only
	matrix[0] =	float(1 - 2*(y2 + z2));
	matrix[1] =	float(2 * (xy + wz));
	matrix[2] =	float(2 * (xz - wy));

	matrix[4] =	float(2 * (xy - wz));
	matrix[5] =	float(1 - 2*(x2 + z2));
	matrix[6] =	float(2 * (yz + wx));

	matrix[8] =	float(2 * (xz + wy));
	matrix[9] =	float(2 * (yz - wx));
	matrix[10] =	float(1 - 2*(x2 + y2));
//

	m.set(matrix);

	return;
}	// end void GetMatrix(..)

////////

void CQuaternion::slerp(const CQuaternion q1, const CQuaternion q2, const float slerp)
{
	//%%%%%%%%%%%%
	// NOT TESTED
	//%%%%%%%%%%%%

	assert(0 <= slerp);
	assert(1.0f >= slerp);

	double	cosom, omega, sinom, scale1, scale2;		// temp vars

	float tx, ty, tz, tw;			// temp vars

	cosom	= q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;	// use dot product to get cos angle between the quaternion

	//%%%%%%%%%%%%%%%%%
	// Flip at start???
	//%%%%%%%%%%%%%%%%%

   if (VISc::MathUtils::EPSILON < 1.0f + cosom)			// if both quaternions are not exactly opposite
	{
		if (VISc::MathUtils::EPSILON > 1.0f - cosom)		// if quaternions are close
		{
			scale1	= 1.0f - slerp;		// use linear interpolation
			scale2	= slerp;
		}
		else		// quaternion not very close
		{
			omega	= acos(cosom);		// use SLERP
			sinom	= sin(omega);

			scale1	= sin((1.0f - slerp) * omega) / sinom;
			scale2	= sin(slerp * omega) / sinom;
		}

		tx	= q2.x;
		ty	= q2.y;
		tz	= q2.z;
		tw	= q2.w;
	}
	else	// quaternions are opposite
	{

		//%%%%%%%%%%%%%%%
		// Is this half?
		//%%%%%%%%%%%%%%%
//		scale1	= sin((1.0f - slerp) * VISc::MathUtils::PI/2);
//		scale2	= sin(slerp * VISc::MathUtils::PI/2);

		scale1	= sin(VISc::MathUtils::PI * (0.5 - slerp));		// calculate a perpendicular quaternion and slerp in that direction
		scale2	= sin(VISc::MathUtils::PI * slerp);

		tx	= -q2.y;
		ty	= q2.x;
		tz	= -q2.w;
		tw	= q2.z;
	}

	x	= float(scale1*q1.x + scale2*tx);
	y	= float(scale1*q1.y + scale2*ty);
	z	= float(scale1*q1.z + scale2*tz);
	w	= float(scale1*q1.w + scale2*tw);

	normalize();

	return;
}	// end void Slerp(..)

////////

CQuaternion CQuaternion::operator*(const CQuaternion &q) const
{
	double rx, ry, rz, rw;		// temp result

	rw	= q.w*w - q.x*x - q.y*y - q.z*z;

	rx	= q.w*x + q.x*w + q.y*z - q.z*y;
	ry	= q.w*y + q.y*w + q.z*x - q.x*z;
	rz	= q.w*z + q.z*w + q.x*y - q.y*x;

	return(CQuaternion((float)rx, (float)ry, (float)rz, (float)rw));
}	// end operator *

/* Screw this method
Quaternion Quaternion::operator*(const Quaternion &q) const
{
	float x,y,z,w;			// temp new values

	Vector3D	v1((*this).x,(*this).y,(*this).z),		// temp vectors from quaternion
				v2(q.x, q.y, q.z);

	float		dot;		// temp dot result

	if (v1.IsZero() || v2.IsZero())		// if either vector components are 0, dot and cross product not defined
	{
		dot = 0;

		v1.Scale(q.w);
		v2.Scale((*this).w);

		v1 += v2;
	}
	else
	{
		Vector3D	nv1, nv2;

		nv1 = v1;
		nv1.Normalize();		// get normalized versions of v1 and v2 for dot product

		nv2 = v2;
		nv2.Normalize();

		dot	= nv1.DotProduct(nv2);	// get dot product

		v1.Scale(q.w);
		v2.Scale((*this).w);

		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// this must also be normalized
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		v1 += v2 + v1*v2;				// s1v2 + s2v1 + v1*v2
	}

	w = (*this).w * q.w - dot;		// s1s2 - v1.v2
	v1.GetValues(x,y,z);			// get the x,y,z values of resultant vector

	return(Quaternion(x,y,z,w));
}	// end operator *
*/
////////

CQuaternion& CQuaternion::operator=(const CQuaternion &q)
{
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;

	return(*this);
}	// end operator =

/////////

CQuaternion& CQuaternion::operator*=(const CQuaternion &)
{
	assert(false);		// make sure nobody calls this yet
	// same as *
	//%%%%%%%%%%%%%%%%%%
	// Unimplemented
	//%%%%%%%%%%%%%%%%%%

	return(*this);
}	// end operator *=


void CQuaternion::print()
{
	qDebug() << endl;
	qDebug() << "Quaternion is " << x << " " << y << " " << z << " w is " << w << endl;
}
