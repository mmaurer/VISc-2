#ifndef ALGEBRA_H
#define ALGEBRA_H

#include <math.h>

namespace VISc
{
   namespace MathUtils
   {
      // Floating Point Library Specific
      const double EPSILON                 = 0.005; // error tolerance for check
      const int    FLOAT_DECIMAL_TOLERANCE = 3; // decimal places for float rounding
      const double PI                      = 3.14159265358979323846; // Pi

      inline double DegreesToRadians(double degree)
      {
         return (degree * PI) / 180.0;
      }

      inline double RadiansToDegrees(double radians)
      {
         return (radians * 180.0) / PI;
      }

      inline double Pow(double x)
      {
         return x * x;
      }

      template <class T>
      inline void Bound(T min, T &value, T max)
      {
         value = (value > max) ? max : ((value < min) ? min : value);
      }

      inline double ClampToZero(double value)
      {
         if (abs(value) < EPSILON)
            return 0.0;

         return value;
      }

      inline bool Equal(double x, double y)
      {
         if (((x - EPSILON) < y) && ((x + EPSILON) > y))
            return true;

         return false;
      }

      inline double Round(double value, double)
      {
         return static_cast<double>(static_cast<int>(value + 0.5));
         //double integer, fraction;

         //fraction = modf(value, &integer); // get fraction and int components

         //return(float(integer + (float(int(fraction * std::pow( * pow( float(10.0), accuracy)))) / pow( float(10.0), accuracy) ) );
      }

      // Component Type
      enum EComponentType { X=0, Y, Z, W };

      EComponentType IntToComponent(int);
      int ComponentToInt(EComponentType);
   }
}

#endif // ALGEBRA_H
