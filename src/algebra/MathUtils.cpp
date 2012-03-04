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
