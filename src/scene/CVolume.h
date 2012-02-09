#ifndef CVOLUME_H
#define CVOLUME_H

#include <stdio.h>
#include <assert.h>

#include <GL/glew.h>     //--Extension Library-->Always include this before  <gl.h>
#include <GL/gl.h>

// Qt classes
#include <QString>
#include <QFile>
#include <QStack>
#include <QtOpenGL/QGLWidget>
#include <QDebug>
#include <QResizeEvent>
#include <QImage>
#include <QDateTime>
#include <QInputDialog>
#include <QLineEdit>

#include "../visc.h"
#include "../algebra/CVector.h"
#include "CVolumeProperties.h"

#if defined(WIN32) && defined(_DEBUG)
#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif

#define ISDIGIT(x) (x >= 48 && x <= 57) ? true : false

class CVolume 
{
	public:
		CVolume();
		CVolume (QString filename);
		CVolume (CVolume &volume);
		CVolume (int, int, int, int);
      // Operator overloading
      CVolume &operator= (CVolume &volume);
      ~CVolume();

		virtual void render();
		virtual void properties();
		virtual void getBoundingBox(float *mw, float *pw, float *mh, float *ph, float *md, float *pd);
      const char *getData() { return this->data.data(); }

		VISc::EVolumeType getType() { return this->volumeType; }
		void setType (VISc::EVolumeType value) { this->volumeType = value; }

		void setName(QString value) { this->name = value; }
		QString getName() { return this->name; }

		void setEnable(bool e) { this->enabled = e; }
		bool getEnable() { return this->enabled; }
		
		QDateTime getTimestamp() { return this->timeStamp; }
		void setTimestamp(QDateTime value) { this->timeStamp = value; }
		
		CVector getPosition() { return this->position; }
		void setPosition(CVector value) { this->position = value; }
		void setPosition(double x, double y, double z) { this->position = CVector(x, y, z); }

		void setAspectRatio(double x, double y, double z) { this->aspectRatio[VISc::dX] = x; this->aspectRatio[VISc::dY] = y; this->aspectRatio[VISc::dZ] = z; }
		void setAspectRatio(VISc::EDirection dir, double value) { this->aspectRatio[dir] = value; }
		double getAspectRatio(VISc::EDirection value) { return this->aspectRatio[value]; }

		void setColor(QColor value) { this->color = value; }
		QColor getColor() { return this->color; }

		void zeroAll() { for (int i = 0; i < totalVoxels(); i++) data[i] = 0; };
		void clear();
		void freeData();
		bool read(QString filename);
		int getVoxel(int);
		int getVoxel(int, int, int);
		double getVoxel(CVector);
		double getVoxelInterpolated(double x, double y, double z);
		void setVoxel(int, int);
		void setVoxel(int, int, int, int);
		void setVoxel(int, int, int, int, int);

		int getWidth() { return this->width; };
		int getHeight() { return this->height; };
		int getDepth() { return this->depth; };
		int getMaxValue() { return this->maxvalue; };
		int getNumComponents() { return this->c; };
		int totalVoxels() { return (this->width * this->height * this->depth); };
		QImage getTexture(int slice, VISc::EDirection direction, GLubyte *);

		CVolume resize(double sx, double sy, double sz, VISc::EInterpolation im);

	private:
		int width, height, depth, c;
		int maxvalue;
		//char *data;
      QScopedArrayPointer<char> data;
		QColor color;
		double aspectRatio[3];
		CVector position;

		bool enabled;
		VISc::EVolumeType volumeType;
		QString name;
		QDateTime timeStamp;

		// Methods
		int stackToNumber(QStack<int> &stack);
};

#endif
