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

/// TODO: correct background color in saved screenshots
/// TODO: Eliminate memory leaks
/// TODO: Use Qt's shaders instead of glut or whatever
/// TODO: Clean up and bring the code up to a finer standard
/// TODO: Keep one copy of every Transfer Widget available to avoid loss of data when changing from one widget to another
/// TODO: Implement a destructor method for every Transfer WIdget (DO NOT IMPLEMENT IN THE DESTRUCTOR) to clean memory every time the transfer widget is changed
/// TODO: Change the way COpenGL and CTransferWidget communicate. Use signals instead of checking readiness (dirty and changed)
/// TODO: Direct Volume Rendering
/// TODO: Comment the code appropriately for Doxygen
/// TODO: Use try ... except clauses (sparingly)
/// TODO: Translate texts
/// TODO: Group classes in namespaces

/// TODO: complete the set of properties:
///		2D Texture Based:
///		add aditional properties
///
///		DVR:
///		connect signals for the step size spinbox
///		add aditional properties

/// HACK: fix viewing direction for perspective view in 2D Texture Based Rendering
/// HACK: fix light direction vector in 2D Texture Based Rendering if volume is translated
/// HACK: improve the size of QDockWidget CMainForm::dock (CMessageBox - make it smaller)

// GL Includes
#include <GL/glew.h>     //--Extension Library-->Always include this before  <gl.h>
#include <GL/gl.h>

// Libraries
//#pragma comment( lib, "opengl32.lib" )									// Search For OpenGL32.lib While Linking
//#pragma comment( lib, "glu32.lib" )										// Search For GLu32.lib While Linking

#pragma comment( lib, "glew32.lib" )
//#pragma comment( lib, "glew32s.lib" ) 

// Includes
#include <iostream>

#include <QApplication>
#include <QSplashScreen>
#include "interface/CMainForm.h"

#if defined(WIN32) && defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
  #endif

int main(int argc, char *argv[])
{
   // Memory leak detection
   #if defined(_MSC_VER) && defined(_DEBUG)
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
   #endif

	QApplication app(argc, argv);
	Q_INIT_RESOURCE(visc);

#ifndef _DEBUG
	QPixmap splashPixmap(":/resource/splash.png");
    QSplashScreen splash(splashPixmap, Qt::WindowStaysOnTopHint);
	splash.show();
#endif

	app.processEvents();

	CMainForm mainForm(NULL, "VISc 2");
	mainForm.show();

#ifndef _DEBUG
   splash.finish(&mainForm);
#endif

	return app.exec();
}
