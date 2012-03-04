/*
  VISc 2 is a simple volume renderer.

  Copyright 2012 Mauricio Maurer (mauricio.maurer@gmail.com)

  This file is part of VISc 2.

  VISc 2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  VISc 2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with VISc 2.  If not, see <http://www.gnu.org/licenses/>.
*/

// Based on the code by Sobeit Void
// ================================================================================================
#ifndef CQUATERNION_H
#define CQUATERNION_H

#include <math.h>

#include "MathUtils.h"
#include "CVector.h"
#include "CMatrix.h"

#if defined(WIN32) && defined(_DEBUG)
   #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
   #define new DEBUG_NEW
#endif

class CQuaternion
{
private:

	float	x,y,z,w;			// quaternion representation (w, xi, yj, zk)

	/*__forceinline*/ inline double norm(void);		// get the norm N^2
	
public:

	CQuaternion();												// constructor a quaternion with zero rotation
	CQuaternion(const CQuaternion &);							// Copy constructor
	CQuaternion(float x, float y, float z, float w);			// constructs a quaternion from given values. w is in rad, and quat will be normalized. Will perform a valid check
	CQuaternion(const float x, const float y, const float z);	// construct a quaternion from euler angles in degrees
	CQuaternion( CVector &v, const float angle);				// construct a quaternion from axis angle representation, angle in radians

//	~CQuaternion();

////////

	void	identity(void);														// set this quaternion to the identity quaternion
	void	zeroClamp(void);													// clamp any values very close to 0
	void	normalize(void);													// normalize this quaternion
	void	getValues(float &x, float &y, float &z, float &w)			const;	// gets the value of this quaternion
	
////////

	void	axisToQuat(CVector &v, const float angle);					// convert an axis angle to quaternion, angle is in radians
	void	eulerToQuat(const float x, const float y, const float z);			// convert euler angles to quaternion, euler in degrees
	void	getAxisAngle(CVector &v, float &angle)						const;	// get an axis angle from this quaternion, angle returned in radians
	void	getEulerAngles(float &x, float &y, float &z)				const;	// get the Euler angles(deg) representing this quaternion
	void	getMatrix(CMatrix &m)										const;	// get the 4x4 rotation matrix representation of this quaternion

////////

	//%%%%%%%%%%%
	// Not Tested
	//%%%%%%%%%%%
	// Slerps and lerps
	void	slerp(const CQuaternion q1, const CQuaternion q2, const float slerp);		// computes spherical linear interpolation between q1 and q2 with slerp 0 - 1

/////////

	// operators
	CQuaternion	operator*(const CQuaternion &q) const; // multiplication
	CQuaternion&	operator=(const CQuaternion &q);								// copy constructor

	//%%%%%%%%%%%%%%%
	// UnImplemented
	//%%%%%%%%%%%%%%%
	CQuaternion&	operator*=(const CQuaternion &q);

//////////

	// debug
	void print(void);

	float getX() const { return x; };
	float getY() const { return y; };
	float getZ() const { return z; };
	float getW() const { return w; };

};
#endif
