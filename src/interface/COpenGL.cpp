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

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <cmath>
#include <typeinfo>

#include <GL/glew.h>     //--Extension Library-->Always include this before  <gl.h>
//#include <GL/gl.h>
#include <GL/glut.h>

#include "COpenGL.h"
#include "CMessageBox.h"

#include <QtGui>
//#include <QtOpenGL/QtOpenGL>
#include <QtDebug>
#include <QGLShaderProgram>
#include <QGLShader>
#include <QGLFramebufferObject>
#include <QMenu>

#include "../visc.h"

// Constructors =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
COpenGL::COpenGL(QWidget *parent) : QGLWidget(parent)
{
	projectionMode = VISc::pParallel;
	renderMode = VISc::rmWireframe;
	interpolationMode = VISc::iLinear;
	boundingBox = true;
	axis = true;
	lights = true;
	eyeScreen = 250;
	interactive = true;
	redrawScene = false;
	interpolationModeEnum = GL_LINEAR;
	sliceNumber = 1;
	threshold = 0.0;
	shininess = 100.0;
	showViewingVector = true;
	backgroundColor = QColor(112, 154, 209, 255);
	saveScreen = false;
	updateInterval = 250;
	currentMouseMode = VISc::mmRotation;
	translationX = translationY = initialX = initialY = 0.0;
	diameter = diameterCorrected = 0.0;
	radius = radiusCorrected = 0.0;
	zoomWidth = 1.5;
	zoomFactor = 2;
	zoomStyle = 0xAAAA;
	zoomColor = QColor(255, 204, 0, 255);
	zoomTextColor = QColor(255, 255, 255, 255);
	displayZoomText = false;
	currentViewMode = VISc::vmNormal;
	displayViewingVector = true;
	viewingVectorColor = QColor(255, 255, 255);

	// GL shader
	ProgramObject = 0;
	VertexShaderObject = 0;
	FragmentShaderObject = 0;

	// Trackball
	myTrackball.setViewportDimensions(this->width(), this->height());

	// Timer Testing
     timer = new QTimer(this);
     connect(timer, SIGNAL(timeout()), this, SLOT(setNewTransferFunction()));
     timer->start(updateInterval);
}
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


// Signals =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void COpenGL::setNewTransferFunction()
{
     updateGL();
}
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// Setter/getter methods =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void COpenGL::setMouseMode(VISc::EMouseMode value)
{ 
	this->currentMouseMode = value; 
	
	switch (value) 
	{ 
	case VISc::mmRotation: 
					this->setCursor(Qt::ArrowCursor);
					break;
	case VISc::mmTranslation: 
					this->setCursor(Qt::OpenHandCursor);
					break;
	case VISc::mmZoom: 
					this->setCursor(Qt::CrossCursor);
					break;
	} 
}
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// Overriden signals =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void COpenGL::paintGL()
{
	if (tmpTransferWidget->isDirtyVolume())
	{
		tmpTransferWidget->setCleanVolume();
		this->bindTextures();
	}

	if (tmpTransferWidget->isChanged())
	{
		tmpTransferWidget->setUnchanged();
		this->setTransferFunction( tmpTransferWidget->getTransferFunction(0) );
	}

	QGLFramebufferObject *fbo;
	int oldWidth, oldHeight;
	if (saveScreen)
	{
		oldWidth = this->width();
		oldHeight = this->height();
		this->resizeViewport(widthImage, heightImage);
		fbo = new QGLFramebufferObject(widthImage, heightImage);
		fbo->bind();
	}

	glClearDepth(1.0);
	glClearColor(backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF(), 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(translationX, translationY, 0.0);
	if (projectionMode == VISc::pPerspective)
		glTranslatef(0.0, 0.0, -eyeScreen);
	myTrackball.multiply();
	
	// Axis
	if ((tmpVolumeList->count() > 0) && axis)
		renderAxis();

	// Tranfer Widget self renderer
	tmpTransferWidget->render();

	if (renderMode == VISc::rmWireframe)
		drawWireframe();
	else if (renderMode == VISc::rmUnrealTime)
		drawVolumeDvr();
	else if (renderMode == VISc::rmRealTime)
	{
		if (lights)
		{
			if (tmpLightList != NULL)
			{
				// The first light doesn't have to be drawn, it is the Ambient Light
				for (int i = 1; i < tmpLightList->count(); i++)
					(*tmpLightList).at(i)->render();
			}
		}

		for (int i = 0; i < VISc::MAXIMUM_NUMBER_OF_VOLUMES; i++)
			if (i < tmpVolumeList->count())
				drawVolume(i);
	}

	if ((currentMouseMode == VISc::mmZoom) && leftButtonPressed)
		drawZoomRectangle();

	glFlush();
	glFinish();

	if (saveScreen)
	{
		fbo->release();
		if (!(fbo->toImage()).save(fileNameImage))
			CMessageBox::getInstance()->writeMessage(tr("Error saving screenshot"), VISc::mtError);
		else
			CMessageBox::getInstance()->writeMessage(tr("Screenshot saved to ") + fileNameImage, VISc::mtInformation);

		saveScreen = false;
		resizeViewport(oldWidth, oldHeight);
	}
}


void COpenGL::resizeGL(int, int)
{
	resizeViewport(0, 0);
}


void COpenGL::initializeGL()
{
	glewInit();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

   //QGLShader fragmentShader(QGLShader::Fragment);
   //QFile fragmentShaderFile("Debug/shaders/tex.frag");
   //if (fragmentShaderFile.exists())
   //{
   //   if (fragmentShader.compileSourceFile("Debug/shaders/tex.frag"))
   //      CMessageBox::getInstance()->writeMessage( "Fragment shader compiled successfully.", VISc::mtInformation);
   //   else
   //      CMessageBox::getInstance()->writeMessage( fragmentShader.log(), VISc::mtError );

   //   QGLShaderProgram program( context(), this );
   //   program.addShader( &fragmentShader );
   //   if (program.link())
   //      CMessageBox::getInstance()->writeMessage( "Fragment shader linked successfully.", VISc::mtInformation );
   //   else
   //      CMessageBox::getInstance()->writeMessage( program.log(), VISc::mtError );
   //}
   //else
   //   CMessageBox::getInstance()->writeMessage( "Fragment shader file not found.", VISc::mtWarning );


	// OpenGL Extensions
	GLchar *FragmentShaderSourceDvrVertex, *FragmentShaderSourceDvrFragment;
	GLchar *FragmentShaderSourceTexVertex, *FragmentShaderSourceTexFragment;
	readShaderSource("dvr", &FragmentShaderSourceDvrVertex, &FragmentShaderSourceDvrFragment);
	readShaderSource("tex", &FragmentShaderSourceTexVertex, &FragmentShaderSourceTexFragment);
	int success = 0;

	success = installShaders(FragmentShaderSourceDvrVertex, FragmentShaderSourceDvrFragment, VISc::rmUnrealTime);
	if (success == 0)
	{
		CMessageBox::getInstance()->writeMessage(tr("Error installing shaders for Direct Volume Rendering"), VISc::mtError);
		//exit(1);
	}

	success = installShaders(FragmentShaderSourceTexVertex, FragmentShaderSourceTexFragment, VISc::rmRealTime);
	if (success == 0)
	{
		CMessageBox::getInstance()->writeMessage(tr("Error installing shaders for 2D Texture Rendering"), VISc::mtError);
		//exit(1);
	}
}

void COpenGL::setTransferFunction(const QImage &texture)
{
	transferFunction = QGLWidget::convertToGLFormat(texture);

	//glEnable(GL_TEXTURE_1D);
	glGenTextures(1, &transferTexture);
	glBindTexture(GL_TEXTURE_1D, transferTexture);

	// (set texture parameters here)
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, transferFunction.width(), 0, GL_RGBA, GL_UNSIGNED_BYTE, transferFunction.bits());
}

void COpenGL::mousePressEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		timer->stop();
		switch (currentMouseMode)
		{
		case VISc::mmRotation:
						myTrackball.setInitialPoint(event->x(), event->y());
						break;
		case VISc::mmTranslation:
		case VISc::mmZoom:
						initialX = event->x();
						initialY = (this->height() - event->y());
						break;
		}
		leftButtonPressed = true;
	}
}

void COpenGL::mouseMoveEvent(QMouseEvent *event)
{
	if ((event->buttons() & Qt::LeftButton))
	{
		switch (currentMouseMode)
		{
		case VISc::mmRotation:
						myTrackball.setLastPoint(event->x(), event->y());
						myTrackball.rotate();
						myTrackball.setInitialPoint(event->x(), event->y());
						break;
		case VISc::mmTranslation:
						float scalex, scaley;
						if (this->height() > this->width())
						{
							scalex = diameter / this->width();
							scaley = diameterCorrected / this->height();
						}
						else
						{
							scalex = diameterCorrected / this->width();
							scaley = diameter / this->height();
						}
						translationX += (event->x() - initialX) * scalex;
						translationY += ((this->height() - event->y()) - initialY) * scaley;
						initialX = event->x();
						initialY = (this->height() - event->y());
						break;
		case VISc::mmZoom:
						currentX = event->x();
						currentY = this->height() - event->y();
						break;
		}
	}
	else
		redrawScene = true;

	updateGL();
}

void COpenGL::mouseReleaseEvent(QMouseEvent *)
{
	if (leftButtonPressed)
	{
		if (currentMouseMode == VISc::mmZoom)
		{
			currentViewMode = VISc::vmZooming;
			resizeViewport(0, 0);
		}

		timer->start();
		redrawScene = true;
		leftButtonPressed = false;
		updateGL();

	}
}

void COpenGL::contextMenuEvent(QContextMenuEvent *event)
{
	contextMenu->exec(mapToGlobal(event->pos()));
}
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


// GL Shaders =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
GLint COpenGL::getUniLoc(GLuint program, const GLchar *name)
{
    GLint loc;

    loc = glGetUniformLocationARB(program, name);

    if (loc == -1)
		CMessageBox::getInstance()->writeMessage(tr("No such uniform named ") + name, VISc::mtError);
        //qDebug() <<  << name << endl;

    printOpenGLError();  // Check for OpenGL errors
    return loc;
}

void COpenGL::printShaderInfoLog(GLuint shader)
{
	int infologLength = 0;
	int charsWritten  = 0;
	GLchar *infoLog;

	printOpenGLError();  // Check for OpenGL errors
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);
	printOpenGLError();  // Check for OpenGL errors

   try
   {
	if (infologLength > 0)
	{
      infoLog = new GLchar[infologLength];
			//infoLog = (GLchar *)malloc(infologLength);
			if (infoLog == NULL)
			{
					CMessageBox::getInstance()->writeMessage(tr("Could not allocate InfoLog buffer"), VISc::mtError);
					//exit(1);
					return;
			}
			glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
         if (charsWritten > 0)
         {
            infoLog[charsWritten - 1] = '\0';
            if (CMessageBox *mb = CMessageBox::getInstance())
               mb->writeMessage(tr("Shader InfoLog: ") + infoLog, VISc::mtInformation);
         }

         if (infoLog != NULL)
            delete [] infoLog;
	}
   }
   catch(...)
   {
   }
	printOpenGLError();  // Check for OpenGL errors
}


void COpenGL::printProgramInfoLog(GLuint program)
{
	int infologLength = 0;
	int charsWritten  = 0;
	GLchar *infoLog;

	printOpenGLError();  // Check for OpenGL errors

	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);

	printOpenGLError();  // Check for OpenGL errors

	if (infologLength > 0)
	{
      infoLog = new GLchar[infologLength];
			//infoLog = (GLchar *)malloc(infologLength);
			if (infoLog == NULL)
			{
					CMessageBox::getInstance()->writeMessage(tr("Could not allocate InfoLog buffer"), VISc::mtError);
					//exit(1);
					return;
			}
			glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
         if (charsWritten > 0)
         {
            infoLog[charsWritten - 1] = '\0';
            CMessageBox::getInstance()->writeMessage(tr("Program InfoLog: ") + infoLog, VISc::mtInformation);
         }

         delete [] infoLog;
	}
	printOpenGLError();  // Check for OpenGL errors
}


int COpenGL::shaderSize(QString fileName, VISc::EProfile shaderType)
{
	//
	// Returns the size in bytes of the shader fileName.
	// If an error occurred, it returns -1.
	//
	// File name convention:
	//
	// <fileName>.vert
	// <fileName>.frag

	switch (shaderType)
	{
		case VISc::pfVertex:
				fileName.append(".vert");
				break;
		case VISc::pfFragment:
				fileName.append(".frag");
				break;
		default:
				CMessageBox::getInstance()->writeMessage(tr("Unknown shader file type"), VISc::mtError);
				exit(1);
				break;
	}

	QFile tmpFile("Debug/shaders/" + fileName);
	return tmpFile.size();
}


int COpenGL::readShader(QString fileName, VISc::EProfile shaderType, char *shaderText, int size)
{
	//
	// Reads a shader from the supplied file and returns the shader in the
	// arrays passed in. Returns 1 if successful, 0 if an error occurred.
	// The parameter size is an upper limit of the amount of bytes to read.
	// It is ok for it to be too big.
	//
	switch (shaderType) 
	{
			case VISc::pfVertex:
					fileName.append(".vert");
					break;
			case VISc::pfFragment:
					fileName.append(".frag");
					break;
			default:
					CMessageBox::getInstance()->writeMessage(tr("Unknown shader file type"), VISc::mtError);
					exit(1);
					break;
	}

	QFile tmpFile("Debug/shaders/" + fileName);
	if (!tmpFile.open(QIODevice::ReadOnly))
		return 0;
		
	if (tmpFile.read(shaderText, size) == -1)
		return 0;
	shaderText[tmpFile.size()] = '\0';

	return tmpFile.size();
}


int COpenGL::readShaderSource(QString fileName, GLchar **vertexShader, GLchar **fragmentShader)
{
    int vSize, fSize;

    //
    // Allocate memory to hold the source of our shaders.
    //
	vSize = shaderSize(fileName, VISc::pfVertex);
    fSize = shaderSize(fileName, VISc::pfFragment);

    if ((vSize == -1) || (fSize == -1))
    {
        CMessageBox::getInstance()->writeMessage(tr("Cannot determine size of the shader."), VISc::mtError);
        //printf("Cannot determine size of the shader %s\n", fileName);
        return 0;
    }

    *vertexShader = (GLchar *) malloc(vSize);
    *fragmentShader = (GLchar *) malloc(fSize);

    //
    // Read the source code
    //
	if (vSize > 0)
		if (!readShader(fileName, VISc::pfVertex, *vertexShader, vSize))
		{

            CMessageBox::getInstance()->writeMessage(tr("Cannot read the vertex shader."), VISc::mtError);
                //printf("Cannot read the file %s.vert\n", fileName);
				return 0;
		}

	if (fSize > 0)
		if (!readShader(fileName, VISc::pfFragment, *fragmentShader, fSize))
		{
            CMessageBox::getInstance()->writeMessage(tr("Cannot read the fragment shader."), VISc::mtError);
                //printf("Cannot read the file %s.frag\n", fileName);
				return 0;
		}

    return 1;
}


int COpenGL::installShaders(const GLchar *vertexProgram, const GLchar *fragmentProgram, VISc::ERenderMode mode)
{
    return 0;

    GLuint vertexShader, fragmentShader;   // handles to objects
    GLint  vertCompiled = 1, fragCompiled = 1;    // status values
    GLint  linked;

    // Create a vertex shader object and a fragment shader object
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Load source code strings into shaders
    glShaderSource(vertexShader, 1, &vertexProgram, NULL);
    glShaderSource(fragmentShader, 1, &fragmentProgram, NULL);

    // Compile the vertex shader, and print out the compiler log file.
	glCompileShader(vertexShader);
    printOpenGLError();  // Check for OpenGL errors
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertCompiled);
    printShaderInfoLog(vertexShader);

    // Compile the vertex shader, and print out the compiler log file.
    glCompileShader(fragmentShader);
    printOpenGLError();  // Check for OpenGL errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragCompiled);
    printShaderInfoLog(fragmentShader);

    if (!vertCompiled || !fragCompiled)
        return 0;

    // Create a program object and attach the two compiled shaders
	if (mode == VISc::rmUnrealTime)
	{
		programDvr = glCreateProgram();
		glAttachShader(programDvr, fragmentShader);

		glLinkProgram(programDvr);
		printOpenGLError();  // Check for OpenGL errors
		glGetProgramiv(programDvr, GL_LINK_STATUS, &linked);
		printProgramInfoLog(programDvr);

		if (!linked)
	        return 0;
	} else if (mode == VISc::rmRealTime)
	{
		// Dvr2
		programTex = glCreateProgram();
		glAttachShader(programTex, fragmentShader);
		glAttachShader(programTex, vertexShader);

		// Link the program object and print out the info log

		glLinkProgram(programTex);
		printOpenGLError();  // Check for OpenGL errors
		glGetProgramiv(programTex, GL_LINK_STATUS, &linked);
		printProgramInfoLog(programTex);

		if (!linked)
			return 0;
	}

    // Install program object as part of current state
    //glUseProgram(programDvr);

    // Set up initial uniform values
		//glUniform4f(getUniLoc(programDvr, "scaleTex"), (volumeData.getWidth()), 
		//	                                             (volumeData.getHeight()), 
		//																							 (volumeData.getDepth()), 1.0);
  //  glUniform3f(getUniLoc(programDvr, "camera"), 0.0, 0.0, -50);
  //  glUniform3f(getUniLoc(programDvr, "volExtentMin"), 0.0, 0.0, 0.0);
		//glUniform3f(glGetUniformLocation(programDvr, "volExtentMin"), 0.0, 0.0, 0.0); 
		//glUniform3f(getUniLoc(programDvr, "volExtentMax"), volumeData.getWidth(), volumeData.getHeight(), volumeData.getDepth());
  //  glUniform1f(getUniLoc(programDvr, "stepsize"), 1.0);

    return 1;
}

 int COpenGL::printOglError(char *file, int line)
{
    //
    // Returns 1 if an OpenGL error occurred, 0 otherwise.
    //
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        //printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        CMessageBox::getInstance()->writeMessage(tr("Error in file"), VISc::mtError);
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


// Auxiliary methods =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Axis
 void COpenGL::renderAxis()
{
	int min, max;
	if (this->width() < this->height())
	{
		min = this->width();
		max = this->height();
	}
	else
	{
		min = this->height();
		max = this->width();
	}

	// Axis will have the length of 8% of smallest dimension of the viewport
	int squareSize = min * 0.08;

	// Offset from viewport border is one fourth of five percent of the smallest viewport dimension
	int offset = (min * 0.05) / 4.0; 

	// OpenGL Matrices
	GLdouble modelMatrix[16];
	GLdouble projMatrix[16];
	GLint viewport[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Final points
	CPoint centerFinal;
	
	// Initial points
	CPoint center(this->width() - (squareSize + offset), (squareSize + offset), 0.0); 	
	unproject(&center, &centerFinal, modelMatrix, projMatrix, viewport);

	// The length of each axis is 5% of the total diameterCorrected
	float length = diameterCorrected * 0.05;
	glPushMatrix();
		glTranslatef(centerFinal.getX(), centerFinal.getY(), centerFinal.getZ());

		glBegin(GL_LINES);
			glColor3f(1.0, 0.0, 0.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(length, 0.0, 0.0);

			glColor3f(0.0, 1.0, 0.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, length, 0.0);

			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 0.0, length);
		glEnd();
	glPopMatrix();
}

void COpenGL::unproject(CPoint *origin, CPoint *destination, GLdouble *model , GLdouble *proj , GLint *view)
{
   CMatrix mModelViewMatrix(model);
   CMatrix mProjectionMatrix(proj);
   //QMatrix4x4 qmModelViewMatrix(model);
   //qmModelViewMatrix = qmModelViewMatrix.transposed();
   //QMatrix4x4 qmProjectionMatrix(proj);
   //qmProjectionMatrix = qmProjectionMatrix.transposed();
   //QMatrix4x4 product = qmModelViewMatrix * qmProjectionMatrix;
   //QMatrix4x4 inverse = product.inverted();

   //QVector4D dest;
   //dest.setX(2 * (origin->getX() - view[0]) / view[2] - 1);
   //dest.setY(2 * (origin->getY() - view[1]) / view[3] - 1);
   //dest.setZ(2 * origin->getZ() - 1);
   //dest.setW(1.0);

   //QVector4D res = dest * inverse;
   //destination->setX( res.x() );
   //destination->setY( res.y() );
   //destination->setZ( res.z() );
   //destination->setW( res.w() );

	// Transforms OpenGL matrix into a CMatrix object
//	CMatrix mModelViewMatrix;
//	CMatrix mProjectionMatrix;
//	mModelViewMatrix.setGlMatrix(model);
//	mProjectionMatrix.setGlMatrix(proj);

	// Invert the matrix to obtain
	CMatrix product = mProjectionMatrix * mModelViewMatrix;
	CMatrix inverse = product.invert();

	destination->setX(2 * (origin->getX() - view[0]) / view[2] - 1);
	destination->setY(2 * (origin->getY() - view[1]) / view[3] - 1);
	destination->setZ(2 * origin->getZ() - 1);
	destination->setW(1.0);

	(*destination) = inverse * (*destination);
}

 // Zoom
 void COpenGL::drawZoomRectangle()
 {
	// Remap the viewing volume to match window coordinates (approximately)
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
			glLoadIdentity();

			// Establish clipping volume (left, right, bottom, top, near, far)
			glOrtho(0, this->width(), 0, this->height(), -1, 1);
			
			// Enable primitive drawing and attributes
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glPushAttrib(GL_ALL_ATTRIB_BITS);
				glDisable(GL_DEPTH_TEST);
				glLineStipple(zoomFactor, zoomStyle);
				glEnable(GL_LINE_STIPPLE);
				glLineWidth(zoomWidth);

				glDisable(GL_LIGHTING);
				glColor3f(zoomColor.redF(), zoomColor.greenF(), zoomColor.blueF());
				glBegin(GL_LINE_LOOP);
					glVertex2i(currentX, currentY);
					glVertex2i(currentX, initialY);
					glVertex2i(initialX, initialY);
					glVertex2i(initialX, currentY);
				glEnd();

				if (displayZoomText)
				{
					//GLdouble projectionMatrix[16];

					//glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix); 
					//CMatrix projMatrix;
					//projMatrix.setGlMatrix(projectionMatrix);

					//CMatrix pMi = projMatrix.invert();
					//CPoint sp(initialX, initialY, 0.0, 0.0);
					//CPoint np = pMi * sp;
					//CPoint fp(currentX, currentY, 0.0, 0.0);	
					//CPoint ep = pMi * fp;
					//int diff;
					//if (renderMode == rmRealTime)
					//	diff = 15;
					//else
					//	diff = 5;
					//renderText(5, this->height() - diff, QString::number(np.getX(),'f', 0) + ", " + QString::number(np.getY(),'f', 0)+ " - " + QString::number(ep.getX(),'f', 0) + ", " + QString::number(ep.getY(),'f', 0)  );
				 	int diff;

					glColor4f(zoomTextColor.redF(), zoomTextColor.greenF(), zoomTextColor.blueF(), 1.0);
					if (renderMode == VISc::rmRealTime)
						diff = 15;
					else
						diff = 5;
					renderText(5, this->height() - diff, QString::number(initialX,'f', 0) + ", " + QString::number(initialY,'f', 0) + " - " + QString::number(currentX,'f', 0) + ", " + QString::number(currentY,'f', 0)  );
				}
			glPopAttrib();

			glMatrixMode(GL_PROJECTION);
		glPopAttrib();
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
 }

// Direct Volume Rendering
void COpenGL::drawVolumeDvr()
{
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glClearColor(backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF(), 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	glTranslatef(0.0, 0.0, -eyeScreen);

//	glTranslatef(-volumeData.getWidth() / 2.0, -volumeData.getHeight() / 2.0, -volumeData.getDepth() / 2.0);

	// Get modelview matrix from Open GL
	//glGetFloatv(GL_MODELVIEW_MATRIX, pModelViewMatrix);
	glGetDoublev(GL_MODELVIEW_MATRIX, pModelViewMatrix);

	// Transforms OpenGL matrix into a CMatrix object
	CMatrix mModelViewMatrix;
	mModelViewMatrix.setGlMatrix((double *) pModelViewMatrix);
	mModelViewMatrix.invert();
	
	CVector viewVector(0.0, 0.0, -1.0);
	CVector viewVectorVSpace = mModelViewMatrix * viewVector;
	//GLfloat pViewVector[4] = {0.0f, 0.0f, -1.0f, 0.0f};
	//MatVecMultiply(pModelViewMatrixInv, pViewVector);
	
	//normalizeVector(pViewVector);
	viewVectorVSpace.normalize();

	//GLfloat cameraPos[4] = {0.0f, 0.0f, eyeScreen, 0.0f};
	CPoint cameraPos(0.0, 0.0, eyeScreen);
	CPoint result = mModelViewMatrix * cameraPos;

	//MatVecMultiply(pModelViewMatrixInv, cameraPos);
	//normalizeVector(cameraPos);

	if (interactive || (!interactive && redrawScene))
	{

		{
			glUseProgram(programDvr);
			//glUniform3f(getUniLoc(programDvr, "scaleTex"), volumeData.getWidth(), volumeData.getHeight(), volumeData.getDepth());
			if (projectionMode == VISc::pParallel)
				glUniform4f(getUniLoc(programDvr, "camera"), threshold, static_cast<GLfloat>(0.85), static_cast<GLfloat>(0.0), static_cast<GLfloat>(1.0));
			else if (projectionMode == VISc::pPerspective)
				glUniform4f(getUniLoc(programDvr, "camera"), threshold, static_cast<GLfloat>(0.85), static_cast<GLfloat>(1.0), static_cast<GLfloat>(1.0));
			//glUniform3f(getUniLoc(programDvr, "volExtentMin"), 0.0, 0.0, 0.0);
			//glUniform3f(getUniLoc(programDvr, "volExtentMax"), volumeData.getWidth(), volumeData.getHeight(), volumeData.getDepth());
			glUniform1f(getUniLoc(programDvr, "stepsize"), 1.0 / (maxVolSize * sliceNumber));
			//glUniform3f(getUniLoc(programDvr, "kakskd"), ka, ks, kd);
			//glUniform3f(getUniLoc(programDvr, "lightPos"), lightPos[0], lightPos[1], lightPos[2]);
			//glUniform3f(getUniLoc(programDvr, "ambient"), ambientColor.redF(), ambientColor.greenF(), ambientColor.blueF());
			//glUniform3f(getUniLoc(programDvr, "diffuse"), lightColor.redF(), lightColor.greenF(), lightColor.blueF());
			glUniform3f(getUniLoc(programDvr, "viewVector"), viewVectorVSpace.getX(), viewVectorVSpace.getY(), viewVectorVSpace.getZ());
			glUniform3f(getUniLoc(programDvr, "cameraPos"), cameraPos.getX(), cameraPos.getY(), cameraPos.getZ());

			// Volume
			glEnable(GL_TEXTURE_3D);
			int volumeLocation = glGetUniformLocationARB(programDvr, "SamplerDataVolume");
			glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_3D, texture3d);
			glUniform1iARB(volumeLocation, 0);

			// Transfer texture
			int transferLocation = glGetUniformLocationARB(programDvr, "SamplerTransferFunction");
			glActiveTexture(GL_TEXTURE1);
			glUniform1iARB(transferLocation, 1);
		}

		// Draw faces of the volume
		drawFacesVolume();

		//glDisable(GL_TEXTURE_3D);
		if (redrawScene)
			redrawScene = !redrawScene;
	}

	if (boundingBox)
		drawBoundingBox();
}



void COpenGL::drawFacesVolume()
{
	int index = 0;
	CVolume *volumeData = tmpVolumeList->at(index).data();

	if (true)
	{
#if 1
		glBegin(GL_QUADS);
			glColor3f(1.0, 1.0, 1.0);
		
			vertex  (0, 0, 0);
			vertex  (0, volumeData->getHeight(), 0);
			vertex  (volumeData->getWidth(), volumeData->getHeight(), 0);
			vertex  (volumeData->getWidth(), 0, 0);

			vertex  (0, 0, 0);
			vertex  (0, 0, volumeData->getDepth());
			vertex  (0, volumeData->getHeight(), volumeData->getDepth());
			vertex  (0, volumeData->getHeight(), 0);
			
			vertex  (volumeData->getWidth(), 0, volumeData->getDepth());
			vertex  (volumeData->getWidth(), 0, 0);
			vertex  (volumeData->getWidth(), volumeData->getHeight(), 0);
			vertex  (volumeData->getWidth(), volumeData->getHeight(), volumeData->getDepth());

			vertex  (volumeData->getWidth(), volumeData->getHeight(), volumeData->getDepth());
			vertex  (volumeData->getWidth(), volumeData->getHeight(), 0);
			vertex  (0, volumeData->getHeight(), 0);
			vertex  (0, volumeData->getHeight(), volumeData->getDepth());
			
			vertex  (volumeData->getWidth(), 0, volumeData->getDepth());
			vertex  (0, 0, volumeData->getDepth());
			vertex  (0, 0, 0);
			vertex  (volumeData->getWidth(), 0, 0);

			vertex  (0, 0, volumeData->getDepth());
			vertex  (volumeData->getWidth(), 0, volumeData->getDepth());
			vertex  (volumeData->getWidth(), volumeData->getHeight(), volumeData->getDepth());
			vertex  (0, volumeData->getHeight(), volumeData->getDepth());
		glEnd();
#else
		glBegin(GL_QUADS);
			glColor4f(backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF(), 1.0);

			glTexCoord3f(0, 0, 0);
			glVertex3f  (0, 0, 0);
			glTexCoord3f(0, 1, 0);
			glVertex3f  (0, volumeData->getHeight(), 0);
			glTexCoord3f(1, 1, 0);
			glVertex3f  (volumeData->getWidth(), volumeData->getHeight(), 0);
			glTexCoord3f(1, 0, 0);
			glVertex3f  (volumeData->getWidth(), 0, 0);

			glTexCoord3f(0, 0, 0);
			glVertex3f(0, 0, 0);
			glTexCoord3f(0, 0, 1);
			glVertex3f(0, 0, volumeData->getDepth());
			glTexCoord3f(0, 1, 1);
			glVertex3f(0, volumeData->getHeight(), volumeData->getDepth());
			glTexCoord3f(0, 1, 0);
			glVertex3f(0, volumeData->getHeight(), 0);
			
			glTexCoord3f(1, 0, 1);
			glVertex3f(volumeData->getWidth(), 0, volumeData->getDepth());
			glTexCoord3f(1, 0, 0);
			glVertex3f(volumeData->getWidth(), 0, 0);
			glTexCoord3f(1, 1, 0);
			glVertex3f(volumeData->getWidth(), volumeData->getHeight(), 0);
			glTexCoord3f(1, 1, 1);
			glVertex3f(volumeData->getWidth(), volumeData->getHeight(), volumeData->getDepth());

			glTexCoord3f(1, 1, 1);
			glVertex3f(volumeData->getWidth(), volumeData->getHeight(), volumeData->getDepth());
			glTexCoord3f(1, 1, 0);
			glVertex3f(volumeData->getWidth(), volumeData->getHeight(), 0);
			glTexCoord3f(0, 1, 0);
			glVertex3f(0, volumeData->getHeight(), 0);
			glTexCoord3f(0, 1, 1);
			glVertex3f(0, volumeData->getHeight(), volumeData->getDepth());
			
			glTexCoord3f(1, 0, 1);
			glVertex3f(volumeData->getWidth(), 0, volumeData->getDepth());
			glTexCoord3f(0, 0, 1);
			glVertex3f(0, 0, volumeData->getDepth());
			glTexCoord3f(0, 0, 0);
			glVertex3f(0, 0, 0);
			glTexCoord3f(1, 0, 0);
			glVertex3f(volumeData->getWidth(), 0, 0);

			glTexCoord3f(0, 0, 1);
			glVertex3f(0, 0, volumeData->getDepth());
			glTexCoord3f(1, 0, 1);
			glVertex3f(volumeData->getWidth(), 0, volumeData->getDepth());
			glTexCoord3f(1, 1, 1);
			glVertex3f(volumeData->getWidth(), volumeData->getHeight(), volumeData->getDepth());
			glTexCoord3f(0, 1, 1);
			glVertex3f(0, volumeData->getHeight(), volumeData->getDepth());
		glEnd();
#endif
	}
}


void COpenGL::vertex(int x, int y, int z)
{
	//if (projectionMode == CPerspective)
	//{
	//	GLfloat pCamera[4] = {volumeData.getWidth() / 2.0, volumeData.getHeight() / 2.0, eyeScreen, 1.0f};
	//	normalizeVector(pCamera);
	//	MatVecMultiply(pModelViewMatrixInv, pCamera);	
	//	v[0] = x - pCamera[0];
	//	v[1] = y - pCamera[1];
	//	v[2] = z - pCamera[2];
	//	normalizeVector(v);
	//}
	//else if (projectionMode == CParallel)
	//{
	//	GLfloat pViewVector[4] = {0.0f, 0.0f, -1.0f, 0.0f};
	//	MatVecMultiply(pModelViewMatrixInv, pViewVector);

	//	normalizeVector(pViewVector);
	//	v[0] = pViewVector[0];
	//	v[1] = pViewVector[1];
	//	v[2] = pViewVector[2];
	//	normalizeVector(v);
	//}

	//v[0] = (v[0] + 1.0) / 2.0;
	//v[1] = (v[1] + 1.0) / 2.0;
	//v[2] = (v[2] + 1.0) / 2.0;
	//glMultiTexCoord3fARB(GL_TEXTURE1_ARB, v[0], v[1], v[2]);
	glMultiTexCoord3fARB(GL_TEXTURE1_ARB, 0, 0, 0);
	float xx, yy, zz;
	if (x > 0) xx = 1.0; else xx = 0.0;
	if (y > 0) yy = 1.0; else yy = 0.0;
	if (z > 0) zz = 1.0; else zz = 0.0;
	glMultiTexCoord3fARB(GL_TEXTURE0_ARB, xx, yy, zz);
	glVertex3f(x, y, z);
}

void COpenGL::saveScreenshot(QString fn, int w, int h)
{
	fileNameImage = fn;
	widthImage = w;
	heightImage = h;
	saveScreen = true;
	redrawScene = true;
	this->updateGL();
}

void COpenGL::setLists(QList<QSharedPointer<CLight> > *lightList, QList<QSharedPointer<CVolume> > *volumeList)
{
	if (lightList != NULL)
		tmpLightList = lightList;
	if (volumeList != NULL)
		tmpVolumeList = volumeList;
}

// Wireframe
void COpenGL::drawWireframe()
{
	if (tmpVolumeList != NULL)
		for (int i = 0; i < tmpVolumeList->count(); i++)
			(*tmpVolumeList).at(i)->render();

	if (lights)
	{
		if (tmpLightList != NULL)
		{
			// The first light is the ambient light and doesn't need to be rendered
			for (int i = 1; i < tmpLightList->count(); i++)
				(*tmpLightList).at(i)->render();
		}
	}
}

float COpenGL::getMaximumDiameter()
{
#define ABS(X) (X < 0 ? X * -1.0 : X)

	float mx, px, my, py, mz, pz;
	//float maxDiameter = 0.0;

	getSceneBoundingBox(&mx, &px, &my, &py, &mz, &pz);
	QList<float> tmpList;

	tmpList.append(mx);
	tmpList.append(px);
	tmpList.append(my);
	tmpList.append(py);
	tmpList.append(mz);
	tmpList.append(pz);
	qSort(tmpList.begin(), tmpList.end());

	if (ABS(tmpList.first()) > ABS(tmpList.back()))
		return 2.0 * sqrt(pow(ABS(tmpList.first()),2.0) + pow(ABS(tmpList.first()) * 1.414, 2.0));
	else
		return 2.0 * sqrt(pow(ABS(tmpList.back()), 2.0) + pow(ABS(tmpList.back())  * 1.414, 2.0));
}

void COpenGL::getSceneBoundingBox(float *minx, float *maxx, float *miny, float *maxy, float *minz, float *maxz)
{
	*maxx = *minx = *maxy = *miny = *maxz = *minz = 0.0;

	for (int i = 0; i < (*tmpVolumeList).count(); i++)
	{
		float mx, px, my, py, mz, pz;
		(*tmpVolumeList).at(i)->getBoundingBox(&mx, &px, &my, &py, &mz, &pz);

		if (mx < *minx)
			*minx = mx;
		if (px > *maxx)
			*maxx = px;

		if (my < *miny)
			*miny = my;
		if (py > *maxy)
			*maxy = py;

		if (mz < *minz)
			*minz = mz;
		if (pz > *maxz)
			*maxz = pz;
	}

	//for (int i = 0; i < (*tmpLightList).count(); i++)
	//{
	//	float mx, px, my, py, mz, pz;
	//	(*tmpLightList).at(i)->getBoundingBox(&mx, &px, &my, &py, &mz, &pz);

	//	if (mx < *minx)
	//		*minx = mx;
	//	if (px > *maxx)
	//		*maxx = px;

	//	if (my < *miny)
	//		*miny = my;
	//	if (py > *maxy)
	//		*maxy = py;

	//	if (mz < *minz)
	//		*minz = mz;
	//	if (pz > *maxz)
	//		*maxz = pz;
	//}
}

void COpenGL::resizeViewport(float width, float height)
{
	//float width = this->width();
	//float height = this->height();
	if (width == 0 || height == 0)
	{
		width = this->width();
		height = this->height();
	}

	//float scale = 1.0;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, width, height);

	// ArcBall
	myTrackball.setViewportDimensions(width, height);

	// Diamater & aspect =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	if (currentViewMode == VISc::vmNormal)
	{
		diameter = getMaximumDiameter();

		if (diameter == 0.0)
			radius = 500.0;
		else
			radius = diameter / 2.0;

		if (height < width)
			diameterCorrected = ((width / height) * diameter);
		else
			diameterCorrected = ((height / width) * diameter);
		
		radiusCorrected = diameterCorrected / 2.0;
		depth = radius;
	}
	else if (currentViewMode == VISc::vmZooming)
	{
		float xprime, yprime, xdprime, ydprime;
		float midx, midy;

		midx = /*this->*/width/*()*/ / 2.0;
		midy = /*this->*/height/*()*/ / 2.0;

		float w, h;

		if (/*this->*/height/*()*/ > /*this->*/width/*()*/)
		{
			w = diameter / /*this->*/width/*()*/;
			h = diameterCorrected / /*this->*/height/*()*/;
		}
		else
		{
			h = diameter / /*this->*/height/*()*/;
			w = diameterCorrected / /*this->*/width/*()*/;
		}

		xprime = (std::min<float>(initialX, currentX) - midx) * w;
      yprime = (std::min<float>(initialY, currentY) - midy) * h;

		xdprime = (std::max<float>(initialX, currentX) - midx) * w;
		ydprime = (std::max<float>(initialY, currentY) - midy) * h;

		float newWidth, newHeight;
		newWidth = xdprime - xprime;
		newHeight   = ydprime - yprime;

		if (newHeight < newWidth)
		{
			biggestSize = newWidth;
			if (/*this->*/height/*()*/ < /*this->*/width/*()*/) // ok
			{
				diameter = newWidth * (/*this->*/height/*()*/ / float(/*this->*/width/*()*/));
				diameterCorrected  = newWidth;
			}
			else // ok
			{
				diameter = newWidth;
				diameterCorrected  = newWidth * (/*this->*/height/*()*/ / float(/*this->*/width/*()*/));
			}
		}
		else
		{
			biggestSize = newHeight;
			if (/*this->*/height/*()*/ < /*this->*/width/*()*/) // ok
			{
				diameter = newHeight;
				diameterCorrected  = newHeight * (/*this->*/width/*()*/ / float(/*this->*/height/*()*/));
			}
			else // ok
			{
				diameter = newHeight * (/*this->*/width/*()*/ / float(/*this->*/height/*()*/));
				diameterCorrected   = newHeight;
			}
		}

		translationX += - (xprime + newWidth / 2.0);
		translationY += - (yprime + newHeight / 2.0);

		radius = diameter / 2.0;
		radiusCorrected = diameterCorrected / 2.0;

		currentViewMode = VISc::vmZoomed;
	}
	else if (currentViewMode == VISc::vmZoomed)
	{
		if (/*this->*/height/*()*/ < /*this->*/width/*()*/)
		{
				diameter = biggestSize;
				diameterCorrected = biggestSize * (/*this->*/width/*()*/ / float(/*this->*/height/*()*/));
		}
		else
		{
				diameter = biggestSize * (/*this->*/width/*()*/ / float(/*this->*/height/*()*/));
				diameterCorrected = biggestSize;
		}

		radius = diameter / 2.0;
		radiusCorrected = diameterCorrected / 2.0;
	}
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

	if (projectionMode == VISc::pParallel)
	{
		if (height < width)
			glOrtho(-radiusCorrected, radiusCorrected, -radius, radius, -depth, depth);
		else
			glOrtho(-radius, radius, -radiusCorrected, radiusCorrected, -depth, depth);
	}
	else
	{
		// Calculating ZNEAR dimensions
		float znearDiameter = ((eyeScreen - radius) * radius) / eyeScreen;
		float znearAspect = ((eyeScreen - radius) * radius) / eyeScreen;
		float updatedAspect = (diameterCorrected) / diameter;

		if (height < width)
			glFrustum(-znearAspect * updatedAspect, znearAspect * updatedAspect, -znearDiameter, znearDiameter, eyeScreen - depth, depth + eyeScreen);
		else
			glFrustum(-znearDiameter, znearDiameter, -znearAspect * updatedAspect, znearAspect * updatedAspect, eyeScreen - depth, depth + eyeScreen);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	update();
}
void COpenGL::drawBoundingBox()
{
	int index = 0;
	CVolume *volumeData = tmpVolumeList->at(index).data();

	if (boundingBox)
	{
		glBegin(GL_LINES);
			glColor4f(volumeData->getColor().redF(), volumeData->getColor().greenF(), volumeData->getColor().blueF(), 1.0);
		
			// Front face
			glVertex3f(0, 0, 0);
			glVertex3f(volumeData->getWidth(), 0, 0);
			glVertex3f(volumeData->getWidth(), 0, 0);
			glVertex3f(volumeData->getWidth(), volumeData->getHeight(), 0);
			glVertex3f(volumeData->getWidth(), volumeData->getHeight(), 0);
			glVertex3f(0, volumeData->getHeight(), 0);
			glVertex3f(0, volumeData->getHeight(), 0);
			glVertex3f(0, 0, 0);

			// Left face
			glVertex3f(0, 0, 0);
			glVertex3f(0, 0, volumeData->getDepth());
			glVertex3f(0, 0, volumeData->getDepth());
			glVertex3f(0, volumeData->getHeight(), volumeData->getDepth());
			glVertex3f(0, volumeData->getHeight(), volumeData->getDepth());
			glVertex3f(0, volumeData->getHeight(), 0);

			// Bottom face
			glVertex3f(volumeData->getWidth(), 0, 0);
			glVertex3f(volumeData->getWidth(), 0, volumeData->getDepth());
			glVertex3f(volumeData->getWidth(), 0, volumeData->getDepth());
			glVertex3f(0, 0, volumeData->getDepth());

			// Right face
			glVertex3f(volumeData->getWidth(), volumeData->getHeight(), 0);
			glVertex3f(volumeData->getWidth(), volumeData->getHeight(), volumeData->getDepth());
			glVertex3f(volumeData->getWidth(), volumeData->getHeight(), volumeData->getDepth());
			glVertex3f(volumeData->getWidth(), 0, volumeData->getDepth());

			// Hinder face
			glVertex3f(0, volumeData->getHeight(), volumeData->getDepth());
			glVertex3f(volumeData->getWidth(), volumeData->getHeight(), volumeData->getDepth());
		glEnd();
	}
}



void COpenGL::perspectiveGL( GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar )
{
	//	Very long (& in theory accurate!) version of Pi. Hopefully an optimizing compiler
	//	will replace references to this with the value!
	const GLdouble pi = 3.1415926535897932384626433832795;

	//	Half of the size of the x and y clipping planes.
	GLdouble fW, fH;

	//	Calculate the distance from 0 of the y clipping plane. Basically trig to calculate
	//	position of clipper at zNear.

	//	Note:	tan( double ) uses radians but OpenGL works in degrees so we convert
	//			degrees to radians by dividing by 360 then multiplying by pi.
	fH = tan( (fovY / 2) / 180 * pi ) * zNear;	
	// Same as fH = tan( fovY / 360 * pi ) * zNear;
	
	//	Calculate the distance from 0 of the x clipping plane based on the aspect ratio.
	fW = fH * aspect;

	//	Finally call glFrustum, this is all gluPerspective does anyway!
	//	This is why we calculate half the distance between the clipping planes - glFrustum
	//	takes an offset from zero for each clipping planes distance. (Saves 2 divides)
	glFrustum( -fW, fW, -fH, fH, zNear, zFar );
}

// Temporary function. Remove...
double hypot(double x,double y)
{
    double t;
    x = abs(x);
    y = abs(y);
    t = std::min(x,y);
    x = std::max(x,y);
    t = t/x;
    return x*sqrt(1+t*t);
}

void COpenGL::lookAtGL(const GLdouble p_EyeX, const GLdouble p_EyeY, const GLdouble p_EyeZ, const GLdouble p_CenterX, const GLdouble p_CenterY, const GLdouble p_CenterZ)
{
	GLdouble l_X = p_EyeX - p_CenterX;
	GLdouble l_Y = p_EyeY - p_CenterY;
	GLdouble l_Z = p_EyeZ - p_CenterZ;

	if(l_X == l_Y && l_Y == l_Z && l_Z == 0.0f)
		return;

	if(l_X == l_Z && l_Z == 0.0f)
	{
		if (l_Y < 0.0f)
			glRotated(-90.0f, 1, 0, 0);
		else
			glRotated(90.0f, 1, 0, 0);
		glTranslated(-l_X, -l_Y, -l_Z);
		return;
	}
  
	GLdouble l_rX = 0.0f;
	GLdouble l_rY = 0.0f;
  
    GLdouble l_hA = (l_X == 0.0f) ? l_Z : hypot(l_X, l_Z);
	GLdouble l_hB;
	if(l_Z == 0.0f)
        l_hB = hypot(l_X, l_Y);
	else
        l_hB = (l_Y == 0.0f) ? l_hA : hypot(l_Y, l_hA);

	l_rX = asin(l_Y / l_hB) * (180 / M_PI);
	l_rY = asin(l_X / l_hA) * (180 / M_PI);

	glRotated(l_rX, 1, 0, 0);
	if(l_Z < 0.0f)
		l_rY += 180.0f;
	else
		l_rY = 360.0f - l_rY;

	glRotated(l_rY, 0, 1, 0);
	glTranslated(-p_EyeX, -p_EyeY, -p_EyeZ);
}


// Real Time
void COpenGL::drawVolume(int index)
{
	CVolume *volumeData = tmpVolumeList->at(index).data();

	glPushMatrix();
	{
		glScalef(volumeData->getAspectRatio(VISc::dX), volumeData->getAspectRatio(VISc::dY), volumeData->getAspectRatio(VISc::dZ));
		glTranslatef(-volumeData->getWidth() / 2.0, -volumeData->getHeight() / 2.0, -volumeData->getDepth() / 2.0);
		glGetDoublev(GL_MODELVIEW_MATRIX, pModelViewMatrix);

		// Transforms OpenGL matrix into a CMatrix object
		CMatrix mModelViewMatrix;
		mModelViewMatrix.setGlMatrix(pModelViewMatrix);
		CMatrix inverse = mModelViewMatrix.invert();
		
		CPoint pViewVector(0.0, 0.0, -1.0, 0.0);
		CPoint viewVectorVSpace = inverse * pViewVector;

		// Output viewing vector into Viewport
		if (displayViewingVector)
		{
			glColor4f(viewingVectorColor.redF(), viewingVectorColor.greenF(), viewingVectorColor.blueF(), 1.0);
			renderText(5, this->height() - 5, QString::number(viewVectorVSpace.getX(),'f', 3) + " " + QString::number(viewVectorVSpace.getY(),'f', 3) + " " + QString::number(viewVectorVSpace.getZ(),'f', 3)  );
		}

		if (boundingBox)
			drawBoundingBox();

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		if (interactive || (!interactive && redrawScene))
		{
			glEnable(GL_BLEND);
			glEnable(GL_ALPHA_TEST);
			glBlendEquationEXT(GL_FUNC_ADD_EXT);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glAlphaFunc(GL_GREATER, threshold);

            glUseProgram(programTex);
			// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
			//glEnable(GL_TEXTURE_3D);	
			int volumeLocation = glGetUniformLocationARB(programTex, "SamplerDataVolume");
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_3D, volumeTexture[index]);
			glUniform1iARB(volumeLocation, 0);
			//printOglError(__FILE__, __LINE__);

			// Transfer texture
			int transferLocation = glGetUniformLocationARB(programTex, "SamplerTransferFunction");
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_1D, transferTexture);
			glUniform1iARB(transferLocation, 1);
			//printOglError(__FILE__, __LINE__);

			// Ligths
			glEnable(GL_LIGHTING);
			for (int i = 0; i < tmpLightList->count(); i++)
			{
				(*tmpLightList).at(i)->setUpLight(i);
				glEnable(GL_LIGHT0 + i);
			}
			int numberLights = glGetUniformLocationARB(programTex, "lights");
			glUniform1iARB(numberLights, tmpLightList->count());
			//printOglError(__FILE__, __LINE__);

			// VolExtentMax
			int volExtentMax = glGetUniformLocationARB(programTex, "volExtentMax");
			glUniform3fARB(volExtentMax, volumeData->getWidth(), volumeData->getHeight(), volumeData->getDepth());

			// View Vector
			int V = glGetUniformLocationARB(programTex, "V");
			glUniform3fARB(V, viewVectorVSpace.getX(), viewVectorVSpace.getY(), viewVectorVSpace.getZ());
			// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

			if (volumeData->getDepth() <= 1)
			{
				glBegin(GL_QUADS);
					glTexCoord3f(0.0, 0.0, 0.5);
					glVertex3f(0.0, 0.0, 0.5);

					glTexCoord3f(1.0, 0.0, 0.5);
					glVertex3f(volumeData->getWidth(), 0.0, 0.5);

					glTexCoord3f(1.0, 1.0, 0.5);
					glVertex3f(volumeData->getWidth(), volumeData->getHeight(), 0.5);

					glTexCoord3f(0.0, 1.0, 0.5);
					glVertex3f(0.0, volumeData->getHeight(), 0.5);
				glEnd();
			}
			else
			{
				switch(viewVectorVSpace.maxComponent()) 
				{
					case VISc::MathUtils::X: if (viewVectorVSpace.getX() > 0)
										drawSliceStack_PositiveX(index);
									else
										drawSliceStack_NegativeX(index);
									break;
					case VISc::MathUtils::Y: if (viewVectorVSpace.getY() > 0)
										drawSliceStack_PositiveY(index);
									else
										drawSliceStack_NegativeY(index);
									break;
					case VISc::MathUtils::Z: if (viewVectorVSpace.getZ() > 0)
										drawSliceStack_PositiveZ(index);
									else
										drawSliceStack_NegativeZ(index);
									break;
               default:
                  break;
				}
			}
			glUseProgram(0);

			if (redrawScene)
				redrawScene = !redrawScene;
		}
		glPopAttrib();
	}
	glPopMatrix();
}


void COpenGL::drawSliceStack_NegativeX(int index)
{
	CVolume *volumeData = tmpVolumeList->at(index).data();

	int nNumSlices = volumeData->getWidth() * sliceNumber;
	double step = volumeData->getWidth() / (double) nNumSlices;

	for (int slice = 0; slice <= nNumSlices; slice++)
	{
		float pos = (step * slice) / float(volumeData->getWidth());
		glBegin(GL_QUADS);
			glTexCoord3f(pos, 0.0, 0.0);
			glVertex3f(step * slice, 0.0, 0.0);

			glTexCoord3f(pos, 0.0, /*volumeData->getDepth()*/ 1.0);
			glVertex3f(step * slice, 0.0, volumeData->getDepth());

			glTexCoord3f(pos, /*volumeData->getHeight()*/ 1.0, /*volumeData->getDepth()*/ 1.0);
			glVertex3f(step * slice, volumeData->getHeight(), volumeData->getDepth());

			glTexCoord3f(pos, /*volumeData->getHeight()*/ 1.0, 0.0);
			glVertex3f(step * slice, volumeData->getHeight(), 0.0);
		glEnd();
	}
}
void COpenGL::drawSliceStack_PositiveX(int index)
{
	CVolume *volumeData = tmpVolumeList->at(index).data();

	int nNumSlices = volumeData->getWidth() * sliceNumber;
	double step = volumeData->getWidth() / (double) nNumSlices;

	for (int slice = nNumSlices; slice >= 0; slice--)
	{
		float pos = (step * slice) / float(volumeData->getWidth());
		glBegin(GL_QUADS);
			glTexCoord3f(pos, 0.0, 0.0);
			glVertex3f(step * slice, 0.0, 0.0);

			glTexCoord3f(pos, 0.0, /*volumeData->getDepth()*/ 1.0);
			glVertex3f(step * slice, 0.0, volumeData->getDepth());

			glTexCoord3f(pos, /*volumeData->getHeight()*/ 1.0, /*volumeData->getDepth()*/ 1.0);
			glVertex3f(step * slice, volumeData->getHeight(), volumeData->getDepth());

			glTexCoord3f(pos, /*volumeData->getHeight()*/ 1.0, 0.0);
			glVertex3f(step * slice, volumeData->getHeight(), 0.0);
		glEnd();
	}
}
void COpenGL::drawSliceStack_NegativeY(int index)
{
	CVolume *volumeData = tmpVolumeList->at(index).data();

	int nNumSlices = volumeData->getHeight() * sliceNumber;
	double step = volumeData->getHeight() / (double) nNumSlices;

	for (int slice = 0; slice <= nNumSlices; slice++)
	{
		float pos = (step * slice) / float(volumeData->getHeight());
		glBegin(GL_QUADS);
			glTexCoord3f(0.0, pos, 0.0);
			glVertex3f(0.0, step * slice, 0.0);

			glTexCoord3f(1.0, pos, 0.0);
			glVertex3f(volumeData->getWidth(), step * slice, 0.0);

			glTexCoord3f(1.0, pos, 1.0);
			glVertex3f(volumeData->getWidth(), step * slice, volumeData->getDepth());

			glTexCoord3f(0.0, pos, 1.0);
			glVertex3f(0.0, step * slice, volumeData->getDepth());
		glEnd();
	}
}
void COpenGL::drawSliceStack_PositiveY(int index)
{
	CVolume *volumeData = tmpVolumeList->at(index).data();

	int nNumSlices = volumeData->getHeight() * sliceNumber;
	double step = volumeData->getHeight() / (double) nNumSlices;

	for (int slice = nNumSlices; slice >= 0; slice--)
	{
		float pos = (step * slice) / float(volumeData->getHeight());
		glBegin(GL_QUADS);
			glTexCoord3f(0.0, pos, 0.0);
			glVertex3f(0.0, step * slice, 0.0);

			glTexCoord3f(1.0, pos, 0.0);
			glVertex3f(volumeData->getWidth(), step * slice, 0.0);

			glTexCoord3f(1.0, pos, 1.0);
			glVertex3f(volumeData->getWidth(), step * slice, volumeData->getDepth());

			glTexCoord3f(0.0, pos, 1.0);
			glVertex3f(0.0, step * slice, volumeData->getDepth());
		glEnd();
	}
}
void COpenGL::drawSliceStack_NegativeZ(int index)
{
	CVolume *volumeData = tmpVolumeList->at(index).data();

	int nNumSlices = volumeData->getDepth() * sliceNumber;
	double step = volumeData->getDepth() / (double) nNumSlices;

	for (int slice = 0; slice <= nNumSlices; slice++)
	{
		float pos = (step * slice) / float(volumeData->getDepth());
		glBegin(GL_QUADS);
			glTexCoord3f(0.0, 0.0, pos);
			glVertex3f(0.0, 0.0, step * slice);

			glTexCoord3f(1.0, 0.0, pos);
			glVertex3f(volumeData->getWidth(), 0.0, step * slice);

			glTexCoord3f(1.0, 1.0, pos);
			glVertex3f(volumeData->getWidth(), volumeData->getHeight(), step * slice);

			glTexCoord3f(0.0, 1.0, pos);
			glVertex3f(0.0, volumeData->getHeight(), step * slice);
		glEnd();
	}
}
void COpenGL::drawSliceStack_PositiveZ(int index)
{
	CVolume *volumeData = tmpVolumeList->at(index).data();

	int nNumSlices = volumeData->getDepth() * sliceNumber;
	double step = volumeData->getDepth() / (double) nNumSlices;

	for (int slice = nNumSlices; slice >= 0; slice--)
	{
		float pos = (step * slice) / float(volumeData->getDepth());
		glBegin(GL_QUADS);
			glTexCoord3f(0.0, 0.0, pos);
			glVertex3f(0.0, 0.0, step * slice);

			glTexCoord3f(1.0, 0.0, pos);
			glVertex3f(volumeData->getWidth(), 0.0, step * slice);

			glTexCoord3f(1.0, 1.0, pos);
			glVertex3f(volumeData->getWidth(), volumeData->getHeight(), step * slice);

			glTexCoord3f(0.0, 1.0, pos);
			glVertex3f(0.0, volumeData->getHeight(), step * slice);
		glEnd();
	}
}

void COpenGL::setVolumeTexture(int index)
{
	CVolume *volume = tmpVolumeList->at(index).data();
	
	// Transforms the volume to a texturable object
	// 3D Textures must have power of 2 dimensions
	float log2 = log10f(2.0f);
	float w = log10f(volume->getWidth()) / log2;
	float h = log10f(volume->getHeight()) / log2;
	float d = log10f(volume->getDepth()) / log2;

	w = powf(2.0f, ceilf(w));
	h = powf(2.0f, ceilf(h));
	d = powf(2.0f, ceilf(d));

	if ((w != volume->getWidth()) ||
		(h != volume->getHeight()) ||
		(d != volume->getDepth()))
		CMessageBox::getInstance()->writeMessage(tr("The dataset has not power of 2 dimensions. The displayed volume will be resized to be transferred to the graphics card."), VISc::mtInformation);
	
	float ws = w / (float) volume->getWidth();
	float hs = h / (float) volume->getHeight();
	float ds = d / (float) volume->getDepth();

	//CVolume volumeData(w, h, d, 4);
    CVolume sourceData(volume->resize(ws, hs, ds, VISc::iTrilinear));
    CVolume volumeData(w, h, d, 4);

	//CVolume volumeData(volume->getWidth(), volume->getHeight(), volume->getDepth(), 4);

#define CLAMP(x, min, max) ((x + max) / 2.0)

	// loop for copying voxels and computing gradient
	for (int z = 0; z < volumeData.getDepth(); z++)
		for (int y = 0; y < volumeData.getHeight(); y++)
			for (int x = 0; x < volumeData.getWidth(); x++)
			{
				int zvalue = (sourceData.getVoxel(x,   y,   z+1) - sourceData.getVoxel(x,   y,   z-1));
				zvalue = CLAMP(zvalue, 0, 256);
				int yvalue = (sourceData.getVoxel(x,   y+1, z)   - sourceData.getVoxel(x,   y-1, z));
				yvalue = CLAMP(yvalue, 0, 256);
				int xvalue = (sourceData.getVoxel(x+1, y,   z)   - sourceData.getVoxel(x-1, y,   z));
				xvalue = CLAMP(xvalue, 0, 256);
				int value = sourceData.getVoxel(x, y, z);

				volumeData.setVoxel(x, y, z, xvalue, 0);
				volumeData.setVoxel(x, y, z, yvalue, 1);
				volumeData.setVoxel(x, y, z, zvalue, 2);
				volumeData.setVoxel(x, y, z, value,  3);
			}


	//// loop for copying voxels and computing gradient
	//for (int z = 0; z < volume->getDepth(); z++)
	//	for (int y = 0; y < volume->getHeight(); y++)
	//		for (int x = 0; x < volume->getWidth(); x++)
	//		{
	//			int zvalue = /*abs*/(volume->getVoxel(x,   y,   z+1) - volume->getVoxel(x,   y,   z-1));
	//			zvalue = CLAMP(zvalue, 0, 256);
	//			int yvalue = /*abs*/(volume->getVoxel(x,   y+1, z)   - volume->getVoxel(x,   y-1, z));
	//			yvalue = CLAMP(yvalue, 0, 256);
	//			int xvalue = /*abs*/(volume->getVoxel(x+1, y,   z)   - volume->getVoxel(x-1, y,   z));
	//			xvalue = CLAMP(xvalue, 0, 256);
	//			int value = volume->getVoxel(x, y, z);

	//			volumeData.setVoxel(x, y, z, xvalue, 0);
	//			volumeData.setVoxel(x, y, z, yvalue, 1);
	//			volumeData.setVoxel(x, y, z, zvalue, 2);
	//			volumeData.setVoxel(x, y, z, value,  3);
	//		}

	//glEnable(GL_TEXTURE_3D);
	glGenTextures(1, &volumeTexture[index]);
	glBindTexture(GL_TEXTURE_3D, volumeTexture[index]);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, interpolationMode);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, interpolationMode);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
   if (typeid(volumeData) == typeid(CVolume))
	   glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, volumeData.getWidth(), volumeData.getHeight(), volumeData.getDepth(), 0, GL_RGBA, GL_UNSIGNED_BYTE, volumeData.getData());
   else if (typeid(volumeData) == typeid(CVolume_))
      glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, volumeData.getWidth(), volumeData.getHeight(), volumeData.getDepth(), 0, GL_RGBA, GL_FLOAT, volumeData.getData());
	printOglError(__FILE__, __LINE__);
	
   QResizeEvent qre(this->size(), this->size());
	emit resizeEvent(&qre);

	updateGL();
}
void COpenGL::bindTextures()
{
	unbindTextures();

	for (int i = 0; i < VISc::MAXIMUM_NUMBER_OF_VOLUMES; i++)
		if (i < tmpVolumeList->count())
			setVolumeTexture(i);
}

void COpenGL::unbindTextures()
{
	for (int i = 0; i < VISc::MAXIMUM_NUMBER_OF_VOLUMES; i++)
	{

	}
}
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
