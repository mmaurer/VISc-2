#ifndef CPOINT_H
#define CPOINT_H

#include "MathUtils.h"

class CVector;

/// Three-dimensional point
/** Represents a three-dimensional point with coordinates x, y and z. Each 
component is of Double type.
*/
class CPoint
{
public:
  // Empty Constructor
  CPoint(); // Default constructor
  CPoint(const CPoint &point); // Copy constructor
  CPoint(const CVector &vector);
  CPoint(double x, double y, double z);
  CPoint(double x, double y, double z, double w);
  
  // Calculates the distance from another point.
  double  distanceToPoint(const CPoint &point);
  
  // Convert Point to Vector
  CVector convertToVector(void);

  // Finds the greater vector component
  VISc::MathUtils::EComponentType maxComponent();

  // Overloaded operators
  CVector operator- (CPoint &point);
  CPoint  operator+ (CVector &vector);
  CPoint  operator- (CVector &vector);
  bool    operator== (CPoint &point);
  bool    operator!= (CPoint &point);

  // Accessor Methods
  double getX() const;
  void setX(double value);
  double getY() const;
  void setY(double value);
  double getZ() const;
  void setZ(double value);
  double getW() const;
  void setW(double value);
  double get(VISc::MathUtils::EComponentType component);
  void set(VISc::MathUtils::EComponentType component, double value);
  
  // Auxiliary methods
  void print();

private:
  // Fields
   double x;
   double y;
   double z;
   double w;
};
#endif //CPOINT_H
