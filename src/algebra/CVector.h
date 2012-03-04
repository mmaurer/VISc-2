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

#ifndef CVECTOR_H
#define CVECTOR_H

#include "MathUtils.h"
#include "CPoint.h"

class CVector 
{
public:
  // Default Constructor
    CVector();
  
  // Constructor with initialization of attributes.
    CVector(double x, double y, double z);
    
  // Copy constructor
  CVector(const CVector &vector);

  // Creates a vector fom two points: pointa minus pointb
    CVector(CPoint &pointA, CPoint &pointB);
  
  // Normalize the vector.
  void normalize();
  
  // Inner Product. Returns the inner product between the two vectors.
  double dotProduct(CVector &vector);
  
  // Cross Product. Return a new vector perpendicular to the others.
  CVector crossProduct(CVector &vector);
    
  // Inverts the vector along the origin.
  void invert();
    
  // Calculates the length of the vector.
  double length();

  // Vector is zero?
  bool isZero(void);

  // Zero vector
  void clear() { x = y = z = 0.0; };

  // Is vector a unit vector?
  bool isUnit(void);
    
  // Calculates the angle to another vector. Result in radians.
  double angleToVector (CVector &vector);

  // Finds the greater vector component
  VISc::MathUtils::EComponentType maxComponent();
  
  // Overloading ADD
  CVector operator+ (CVector &vector);
  
  // Overloading MINUS
  CVector operator- (CVector &vector);
    
  // Overloading ADD to POINT
  CVector operator+ (const CPoint &point);
      
  // Overloading SUB to POINT
  CVector operator- (const CPoint &point);
    
  // Overloading EQUAL
  bool operator== (CVector &vector);

	// Overloading DIFFERENT
  bool operator!= (CVector &vector);  
  // Overloading =
  //CVector operator= (CVector &vector);
    
  // Overloading MULTIPLY by SCALAR
  CVector operator* (double scalar);
    
  // Overloading DIVISION by SCALAR.
  CVector operator/ (double scalar);
    
  // Converts the vector to a point
  CPoint convertToPoint();

	void print();
 
  // Accessor Methods
  void set(double xv, double yv, double zv) { x = xv; y = yv; z = zv; }
  double getX() const;
  void setX(double value );
  double getY() const;
  void setY(double value );
  double getZ() const;
  void setZ(double value);
  double get(VISc::MathUtils::EComponentType component);
  double set(VISc::MathUtils::EComponentType component, double value);

private:
  // Fields
   double x;
   double y;
   double z;
};
#endif //CVECTOR_H
