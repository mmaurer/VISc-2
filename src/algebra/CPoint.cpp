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

#include <math.h>
#include <iostream>
using namespace std;

#include "CPoint.h"
#include "CVector.h"

#include <QtDebug>

// Constructors/Destructors
CPoint::CPoint()
{
	this->x = 0.0;
	this->y = 0.0;
	this->z = 0.0;
	this->w = 1.0;
}
CPoint::CPoint(const CPoint &point)
{
	x = point.getX();
	y = point.getY();
	z = point.getZ();
	w = point.getW();
}
CPoint::CPoint(const CVector &vector)
{
	this->x = vector.getX();
	this->y = vector.getY();
	this->z = vector.getZ();
	this->w = 1.0;
}
CPoint::CPoint(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = 1.0;
}
CPoint::CPoint(double x, double y, double z, double w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

// Methods
double CPoint::getX() const
{
	return x;
}
void CPoint::setX(double value)
{
	x = value;  
}
double CPoint::getY() const
{
	return y;  
}
void CPoint::setY(double value)
{
	this->y = value;  
}
double CPoint::getZ() const
{
	return z;  
}
void CPoint::setZ(double value)
{
	this->z = value;  
}
double CPoint::getW() const
{
	return this->w;  
}
void CPoint::setW(double value)
{
	this->w = value;  
}
// Calculates the distance from another point.
double CPoint::distanceToPoint(const CPoint &point)
{
	return sqrt(pow(point.getX() - this->x, 2.0) + 
		    pow(point.getY() - this->y, 2.0) + 
		    pow(point.getZ() - this->z, 2.0));  
}
// Convert Point to Vector
CVector CPoint::convertToVector()
{
	CVector newVector;

	newVector.setX(this->getX());
	newVector.setY(this->getY());
	newVector.setZ(this->getZ());
	
	return newVector;
}
// Finds the greater vector component
VISc::MathUtils::EComponentType CPoint::maxComponent()
{
#define ABS(x) ((x >= 0) ? x : -x)

	double x = ABS(this->x);
	double y = ABS(this->y);
	double z = ABS(this->z);
	
	if (x > y && x > z)
		return VISc::MathUtils::X;
	else if (y > x && y > z)
		return VISc::MathUtils::Y;
	else if (z > x && z > y)
		return VISc::MathUtils::Z;

	return VISc::MathUtils::X;
}
// Overload MINUS
CVector CPoint::operator- (CPoint &point)
{
	CVector vector;

	vector.setX(this->getX() - point.getX());
	vector.setY(this->getY() - point.getY());
	vector.setZ(this->getZ() - point.getZ());

	return vector;
}
CPoint CPoint::operator+ (CVector &vector)
{
	CPoint newPoint;
	
	newPoint.setX(this->getX() + vector.getX());
	newPoint.setY(this->getY() + vector.getY());
	newPoint.setZ(this->getZ() + vector.getZ());
	
	return newPoint;
}
CPoint CPoint::operator- (CVector &vector)
{
	CPoint newPoint;
	
	newPoint.setX(this->getX() - vector.getX());
	newPoint.setY(this->getY() - vector.getY());
	newPoint.setZ(this->getZ() - vector.getZ());
	
	return newPoint;
}
bool CPoint::operator== (CPoint &point)
{
	if (this->x != point.getX())
		return false;
	if (this->y != point.getY())
		return false;
	if (this->z != point.getZ())
		return false;

	return true;
}

bool CPoint::operator!= (CPoint &point)
{
	if (this->x == point.getX())
		return false;
	if (this->y == point.getY())
		return false;
	if (this->z == point.getZ())
		return false;

	return true;
}


// Sets a component of point
void CPoint::set(VISc::MathUtils::EComponentType component, double value)
{
	switch (component)
	{
		case VISc::MathUtils::X: this->x = value; break;
		case VISc::MathUtils::Y: this->y = value; break;
		case VISc::MathUtils::Z: this->z = value; break;
		case VISc::MathUtils::W: this->w = value; break;
	}
}
// Gets a component of point
double CPoint::get(VISc::MathUtils::EComponentType component)
{
	switch (component)
	{
		case VISc::MathUtils::X: return this->x; break;
		case VISc::MathUtils::Y: return this->y; break;
		case VISc::MathUtils::Z: return this->z; break;
		case VISc::MathUtils::W: return this->w; break;
	}
	return 0.0;
}

// Prints point contents
void CPoint::print()
{
	qDebug() << this->x << "\t" << this->y << "\t" << this->z << "\t" << this->w << endl;
}
