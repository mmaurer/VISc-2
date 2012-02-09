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
