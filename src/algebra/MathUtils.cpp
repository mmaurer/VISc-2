#include <exception>
using namespace std;

#include "MathUtils.h"

namespace VISc
{
   namespace MathUtils
   {
      int ComponentToInt(EComponentType component)
      {
         switch (component)
         {
         case X: return 0; break;
         case Y: return 1; break;
         case Z: return 2; break;
         case W: return 3; break;
         default: return 0; break;
         }
      }

      EComponentType IntToComponent(int i)
      {
         switch (i)
         {
         case 0: 
            return X;
         case 1: 
            return Y;
         case 2:
            return Z;
         case 3:
            return W;
         default:
            std::exception e;
            throw e;
         }
      }
   }
}