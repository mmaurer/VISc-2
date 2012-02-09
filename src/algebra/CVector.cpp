#include <math.h>

#include "CVector.h"
#include "CPoint.h"

#include <QtDebug>

CVector::CVector()
{
	this->x = 0.0;
	this->y = 0.0;
	this->z = 0.0;
}
// Constructor with initialization of attributes.
CVector::CVector(double x, double y, double z) 
{
  	this->x = x;
	this->y = y;
	this->z = z;
}
// Copy constructor
CVector::CVector(const CVector &vector)
{
	this->x = vector.getX();  
	this->y = vector.getY();
	this->z = vector.getZ();
}
// Creates a vector fom two points: pointa minus pointb
CVector::CVector (CPoint &pointA, CPoint &pointB)
{
	this->x = pointA.getX() - pointB.getX();
	this->y = pointA.getY() - pointB.getY();
	this->z = pointA.getZ() - pointB.getZ();
}

double CVector::getX() const 
{
	return this->x;  
}
void CVector::setX(double value) 
{
  this->x = value;
}
double CVector::getY() const
{
	return this->y;
}
void CVector::setY(double value) 
{
 	this->y = value;
}
double CVector::getZ( ) const 
{
 	return this->z;
}
void CVector::setZ(double value) 
{
  this->z = value;
}
// Normalize the vector.
void CVector::normalize ()
{
	double length = this->length();  
	if (length == 0.0)
		return;
	this->x = this->x / length;
	this->y = this->y / length;
	this->z = this->z / length;
}
// Inner Product. Returns the inner product between the two vectors.
double CVector::dotProduct (CVector &vector)
{
  return (this->x * vector.getX() + 
	      this->y * vector.getY() +
	      this->z * vector.getZ());
}
// Cross Product. Return a new vector perpendicular to the others.
CVector CVector::crossProduct (CVector &vector)
{
	CVector newVector;
	
	newVector.setX(this->getY() * vector.getZ() - this->getZ() * vector.getY());
	newVector.setY(this->getZ() * vector.getX() - this->getX() * vector.getZ());
	newVector.setZ(this->getX() * vector.getY() - this->getY() * vector.getX());
	
  return newVector;
}
// Inverts the vector along the origin.
void CVector::invert ()
{
	this->x = -1 * this->x;
	this->y = -1 * this->y;
	this->z = -1 * this->z;
}
// Calculates the length of the vector.
double CVector::length ()
{
	return sqrt(pow(this->x, 2.0) + pow(this->y, 2.0) + pow(this->z, 2.0));  
}
// Is vector zero?
bool CVector::isZero(void)
{
	return( VISc::MathUtils::Equal(0.0f, length()) );
}
// Is vector a unit vector?
bool CVector::isUnit(void)
{
	return( VISc::MathUtils::Equal(1.0f, length() ) );
}	// end int IsUnit()

// Calculates the angle to another vector. Result in radians.
double CVector::angleToVector (CVector &vector)
{
	return acos(this->dotProduct(vector) / (this->length() * vector.length()));
}

VISc::MathUtils::EComponentType CVector::maxComponent()
{
	if (this->x > this->y && this->x > this->y)
		return VISc::MathUtils::X;
	else if (this->y > this->x && this->y > this->z)
		return VISc::MathUtils::Y;
	else if (this->z > this->x && this->z > this->y)
		return VISc::MathUtils::Z;
	
	return VISc::MathUtils::X;
}
// Overloading ADD
CVector CVector::operator+ (CVector &vector)
{
	CVector newVector;

	newVector.setX(this->x + vector.getX());
	newVector.setY(this->y + vector.getY());
	newVector.setZ(this->z + vector.getZ());

	return newVector;
}
CVector CVector::operator- (CVector &vector)
{
	CVector newVector;

	newVector.setX(this->x - vector.getX());
	newVector.setY(this->y - vector.getY());
	newVector.setZ(this->z - vector.getZ());

	return newVector;  
}
// Overloading ADD to POINT
CVector CVector::operator+ (const CPoint &point)
{
	CVector newVector;

	newVector.setX(this->x + point.getX());
	newVector.setY(this->y + point.getY());
	newVector.setZ(this->z + point.getZ());

	return newVector;  
}
// Overloading SUB to POINT
CVector CVector::operator- (const CPoint &point) 
{
	CVector newVector;

	newVector.setX(this->x - point.getX());
	newVector.setY(this->y - point.getY());
	newVector.setZ(this->z - point.getZ());

	return newVector;    
}
// Overloading EQUAL
bool CVector::operator== (CVector &vector)
{
	if (this->x != vector.getX())
		return false;
	if (this->y != vector.getY())
		return false;
	if (this->z != vector.getZ())
		return false;

	return true;
}

// Overloading =
//CVector CVector::operator= (CVector &vector)
//{
//	CVector newVector;
//
//	newVector.setX(vector.getX());
//	newVector.setY(vector.getY());
//	newVector.setZ(vector.getZ());
//
//	return newVector;  
//}
// Overloading MULTIPLY by SCALAR
CVector CVector::operator* (double scalar)
{
	CVector newVector;

	newVector.setX(this->x * scalar);
	newVector.setY(this->y * scalar);
	newVector.setZ(this->z * scalar);

	return newVector;    
}
// Overloading DIVISION by SCALAR.
CVector CVector::operator/ (double scalar)
{
	CVector newVector;

	if (VISc::MathUtils::Equal(scalar, 0.0))
		newVector.set(this->x, this->y, this->z);
	else
	{
		newVector.setX(this->x / scalar);
		newVector.setY(this->y / scalar);
		newVector.setZ(this->z / scalar);
	}

	return newVector;      
}
// Converts the vector to a point
CPoint CVector::convertToPoint()
{
	CPoint newPoint;

	newPoint.setX(x);
	newPoint.setY(y);
	newPoint.setZ(z);

	newPoint.setW(1.0);

	return newPoint;
}
// Gets a defined component from the vector
double CVector::get(VISc::MathUtils::EComponentType component)
{
	switch(component)
	{
		case VISc::MathUtils::X: return this->x; break;
		case VISc::MathUtils::Y: return this->y; break;
		case VISc::MathUtils::Z: return this->z; break;
		default:
         std::exception e;
         throw e;
	}
}
// Sets a defined component of the vector
double CVector::set(VISc::MathUtils::EComponentType component, double value)
{
	switch (component)
	{
		case VISc::MathUtils::X: this->x = value; break;
		case VISc::MathUtils::Y: this->y = value; break;
		case VISc::MathUtils::Z: this->z = value; break;
		default: 
         std::exception e;
         throw e; 
         break;
	}
	return value;
}
void CVector::print()
{
	qDebug() << this->x << "\t" << this->y << "\t" << this->z << endl;
}
