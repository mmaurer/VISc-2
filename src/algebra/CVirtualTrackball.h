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

#ifndef CVIRTUALTRACKBALL_H
#define CVIRTUALTRACKBALL_H

#include <GL/glew.h>     //--Extension Library-->Always include this before  <gl.h>
//#include <GL/glut.h>     //--glut. need to install this to be able to show a window on the screen
#include <GL/gl.h>

#include "CVector.h"
#include "CQuaternion.h"

#if defined(WIN32) && defined(_DEBUG)
   #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
   #define new DEBUG_NEW
#endif

class CVirtualTrackball
{
private:
	int width, height;
	int max;
	int initx, inity;
	int lastx, lasty;
	CQuaternion currentRotation;

public:
	CVirtualTrackball();
	CVirtualTrackball(int, int);

	void setViewportDimensions(int, int);
	void setInitialPoint(int, int);
	void setLastPoint(int, int);
	
	void rotate(void);
	void multiply(void);
	void reset();
};
#endif
