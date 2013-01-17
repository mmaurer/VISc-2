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

#ifndef COPENGL_H
#define COPENGL_H

//#include <GL/glew.h>     //--Extension Library-->Always include this before  <gl.h>
//#include <GL/gl.h>

#include <QtOpenGL/QGLWidget>
#include <QGLFunctions>
#include <QDebug>
#include <QResizeEvent>
#include <QLineEdit>
#include <QList>
#include <QTimer>

#include "../scene/CVolume.h"
#include "../scene/CLight.h"
#include "../scene/COmniLight.h"

#include "../visc.h"
#include "../algebra/CVirtualTrackball.h"
#include "../algebra/MathUtils.h"
#include "../interface/CBasicTransferWidget.h"

#define printOpenGLError() printOglError(__FILE__, __LINE__)

void cgErrorCallback(void);

#if defined(WIN32) && defined(_DEBUG)
   #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
   #define new DEBUG_NEW
#endif

class COpenGL : public QGLWidget, protected QGLFunctions
{
  Q_OBJECT
  
public:
	COpenGL(QWidget *parent /*int timerInterval=0, QWidget *parent=0, char *name=0 */);

	void setProjectionMode(VISc::EProjection projection) { this->projectionMode = projection; emit resizeEvent(new QResizeEvent(this->size(), this->size())); };
	VISc::EProjection getProjectionMode() { return this->projectionMode; };
	
	void setRenderMode(VISc::ERenderMode render) 
   { 
      this->renderMode = render; 
      if (render == VISc::rmWireframe) 
         timer->stop(); 
      else 
         timer->start(); 
      
      QResizeEvent qre(this->size(), this->size());
      emit resizeEvent(&qre);
   };
	VISc::ERenderMode getRenderMode() { return this->renderMode; };
	
	void setInterpolationMode(VISc::EInterpolation interpolation) { this->interpolationMode = interpolation; if (interpolation == VISc::iLinear) interpolationModeEnum = GL_LINEAR; else if (interpolation == VISc::iNearestNeighbor) interpolationModeEnum = GL_NEAREST; };
	VISc::EInterpolation getInterpolarionMode() { return this->interpolationMode; };

	void setEyeScreen(float value) { this->eyeScreen = value; emit resizeEvent(new QResizeEvent(this->size(), this->size())); };
	float getEyeScreen() { return this->eyeScreen; };
	
	void setSliceNumber(int value) { this->sliceNumber = value; };
	int getSliceNumber() { return this->sliceNumber; };

   void setLists(QList<QSharedPointer<CLight> > *, QList<QSharedPointer<CVolume> > *);

	void setContextMenu(QMenu *menu) { this->contextMenu = menu; };

	void setInteractive (bool value) { this->interactive = value; };
	
	double getShininess() { return this->shininess; };
	void setShininess(double value) { this->shininess = value; };

	void setThreshold(float value) { this->threshold = value; };
	float getThreshold() { return this->threshold; };

	void setBackgroundColor(QColor value) { this->backgroundColor = value; };
	QColor getBackgroundColor() { return this->backgroundColor; };

	void setDrawBoundingBox(bool value) { this->boundingBox = value; };
	bool getDrawBoundingBox() { return this->boundingBox; };

	void setDrawAxis(bool value) { this->axis = value; };
	bool getDrawAxis() { return this->axis; };

	void setDrawLights(bool value) { this->lights = value; };
	bool getDrawLights() { return this->lights; };

	void setUpdateInterval(int value) { this->updateInterval = value; };
	int getUpdateInterval() { return this->updateInterval; };

	void setDrawViewingVector(bool value) { this->showViewingVector = value; };
	bool getDrawViewingVector() { return this->showViewingVector; };

	void setMouseMode(VISc::EMouseMode value);
	VISc::EMouseMode getMouseMode() { return this->currentMouseMode; };

	void setZoomWidth(float value) { this->zoomWidth = value; };
	float getZoomWidth() { return this->zoomWidth; };

	void setZoomFactor(int value) { this->zoomFactor = value; };
	int getZoomFactor() { return this->zoomFactor; };

	void setZoomStyle(int value) { this->zoomStyle = value; };
	int getZoomStyle() { return this->zoomStyle; };

	void setZoomColor(QColor value) { this->zoomColor = value; };
	QColor getZoomColor() { return this->zoomColor; };

	void setZoomTextColor(QColor value) { this->zoomTextColor = value; };
	QColor getZoomTextColor() { return this->zoomTextColor; };

	void setDisplayZoomText(bool value) { this->displayZoomText = value; };
	bool getDisplayZoomText() { return this->displayZoomText; };

	void setCurrentViewMode(VISc::EViewportMode value) { this->currentViewMode = value; resizeViewport(0, 0); };
	VISc::EViewportMode getCurrentViewMode() { return this->currentViewMode; };

	void setDisplayViewingVector(bool value) { this->displayViewingVector = value; };
	bool getDisplayViewingVector() { return this->displayViewingVector; }; 

	void setViewingVectorColor(QColor value) { this->viewingVectorColor = value; };
	QColor getViewingVectorColor() { return this->viewingVectorColor; };

	void resetTranslation() { this->translationX = this->translationY = 0; };
	void getTranslation(int *x, int*y) { *x = int(this->translationX); *y = int(this->translationY); };
	void setTranslation(int x, int y) { this->translationX = x; this->translationY = y; };

	void resetRotation() { this->myTrackball.reset(); };

	void setTransferWidget(CBasicTransferWidget *value) { this->tmpTransferWidget = value; }

	void setVolumeTexture(int);
	void bindTextures();
	void unbindTextures();
	void saveScreenshot(QString, int, int);

	void rotateByMouseCoordinates(int *initial, int *final);
	int  loadProgram(QString fileName, VISc::EProfile profile);
	void setTransferFunction(const QImage &texture);
	void resizeViewport(float, float);
	void initNewImage();

	void setRedraw() { this->redrawScene = true; };

protected:
	void initializeGL();
	void resizeGL( int width, int height );
	void paintGL();
  
	//void keyPressEvent( QKeyEvent *e ) { };

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

	void contextMenuEvent(QContextMenuEvent *event);
    
private slots:
	void setNewTransferFunction();


private:
	// Methods
	void perspectiveGL(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar );
	void lookAtGL(const GLdouble p_EyeX, const GLdouble p_EyeY, const GLdouble p_EyeZ, const GLdouble p_CenterX, const GLdouble p_CenterY, const GLdouble p_CenterZ);
	int  FindAbsMaximum(float *pViewVector);
	void drawVolume(int);
	void drawVolumeDvr();
	void drawWireframe();
	void drawFacesVolume();
	void drawSliceStack_NegativeX(int);
	void drawSliceStack_PositiveX(int);
	void drawSliceStack_NegativeY(int);
	void drawSliceStack_PositiveY(int);
	void drawSliceStack_NegativeZ(int);
	void drawSliceStack_PositiveZ(int);
	void vertex(int,int,int);
	float getMaximumDiameter();
	void getSceneBoundingBox(float *, float *, float *, float *, float *, float *);
	void drawBoundingBox();
	void drawZoomRectangle();
	void renderAxis();
    void unproject(CPoint *, CPoint *, GLdouble *, GLdouble *, GLint *);

	// Maximum value found in the volume data
	int maxVolSize;
	
	// Shader -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    QGLShaderProgram m_shaderProgram;
	GLuint ProgramObject;
	GLuint VertexShaderObject;
	GLuint FragmentShaderObject;
	GLint getUniLoc(GLuint program, const GLchar *name);
	GLuint programTex, programDvr;
	GLdouble pModelViewMatrix[16];
	GLfloat pModelViewMatrixInv[16];
	
	void printShaderInfoLog(GLuint shader);
	void printProgramInfoLog(GLuint program);
	int shaderSize(QString fileName, VISc::EProfile shaderType);
	int readShader(QString fileName, VISc::EProfile shaderType, char *shaderText, int size);
	int readShaderSource(QString fileName, GLchar **vertexShader, GLchar **fragmentShader);
	int installShaders(const GLchar *, const GLchar *, VISc::ERenderMode);
	int printOglError(char *file, int line);
	// end shader section =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// OpenGL textures =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	GLuint transferTexture;  // Transfer function texture
	GLuint volumeTexture[VISc::MAXIMUM_NUMBER_OF_VOLUMES];
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

	QColor backgroundColor;

	// Images & Volume =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	QImage transferFunction;
	CBasicTransferWidget *tmpTransferWidget;
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


	// My virtualtrackball =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	CVirtualTrackball myTrackball;
	
	bool interactive;
	bool redrawScene;
	bool leftButtonPressed;
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

	// Volume
	GLenum interpolationModeEnum;
	int sliceNumber;	// Distance between slices
	float threshold;	// Gradient threshold
	bool lights;		// Draw Lights?
	bool boundingBox;	// Draw bounding box?
	bool axis;          // Draw axis?
	QPoint lastPos;		// Mouse capture event
	QTimer *timer;		// Timer
	float eyeScreen;	// Viewpoint
	double shininess;	// n exponent in Phong shading equation
	bool showViewingVector; // Display viewing vector components
	int updateInterval; // Update interval in milliseconds
	VISc::EMouseMode currentMouseMode; // Rotation, Translation or Zoom?
	float translationX, translationY, initialX, initialY, currentX, currentY;
	float diameter, diameterCorrected; // diamter of the sphere circumscribing the scene
	float radius, radiusCorrected;     // related to the previous property
	float depth, biggestSize;		   // depth of view for viewing volumes 
	int zoomStyle, zoomFactor; // Used with glLineStipple
	float zoomWidth;		   // glLineWidth
	QColor zoomColor;          // Zoom rectangle color
	QColor zoomTextColor;
	bool displayZoomText;
	VISc::EProjection projectionMode;
	VISc::ERenderMode renderMode;
	VISc::EInterpolation interpolationMode;
	VISc::EViewportMode currentViewMode;
	bool displayViewingVector;
	QColor viewingVectorColor;


	// Screenshot =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	QString fileNameImage;
	int widthImage, heightImage;
	bool saveScreen;
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

	// Volumes and list // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	QList<QSharedPointer<CLight> > *tmpLightList;
	QList<QSharedPointer<CVolume> > *tmpVolumeList;
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

	// Menu
	QMenu *contextMenu;
};

#endif
