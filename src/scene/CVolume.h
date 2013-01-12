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

#ifndef CVOLUME_H
#define CVOLUME_H

#include <stdio.h>
#include <assert.h>

#include "../visc.h"

//#include <GL/glew.h>     //--Extension Library-->Always include this before  <gl.h>
//#include <GL/gl.h>

// Qt classes
#include <QString>
#include <QFile>
#include <QStack>
#include <QtOpenGL>
#include <QtOpenGL/QGLWidget>
#include <QDebug>
#include <QResizeEvent>
#include <QImage>
#include <QDateTime>
#include <QInputDialog>
#include <QLineEdit>
#include <QVector4D>
#include <QVector3D>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QSpinBox>

#include "../algebra/CVector.h"
#include "CSceneObject.h"

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
        CVolume (const CVolume &volume);
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
		
        CVector getPosition() const { return this->position; }
		void setPosition(CVector value) { this->position = value; }
		void setPosition(double x, double y, double z) { this->position = CVector(x, y, z); }

		void setAspectRatio(double x, double y, double z) { this->aspectRatio[VISc::dX] = x; this->aspectRatio[VISc::dY] = y; this->aspectRatio[VISc::dZ] = z; }
		void setAspectRatio(VISc::EDirection dir, double value) { this->aspectRatio[dir] = value; }
        double getAspectRatio(VISc::EDirection value) const { return this->aspectRatio[value]; }

		void setColor(QColor value) { this->color = value; }
        QColor getColor() const { return this->color; }

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
        int totalVoxels() const { return (this->width * this->height * this->depth); };
		QImage getTexture(int slice, VISc::EDirection direction, GLubyte *);

		CVolume resize(double sx, double sy, double sz, VISc::EInterpolation im);

	private:
		int width, height, depth, c;
		int maxvalue;
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

class CVolume_ : public CSceneObject
{
public:
   CVolume_();
   CVolume_(int width, int height, int depth, int numComponents);
   CVolume_ (const CVolume_ &volume);
   CVolume_ &operator= (const CVolume_ &volume);
   ~CVolume_();

   virtual void render();
   virtual void properties();
   virtual void getBoundingBox(float &mw, float &pw, float &mh, float &ph, float &md, float &pd);

   float getVoxel(int pos) const;
   float getVoxel(int x, int y, int z) const;
   float getVoxel(int x, int y, int z, int component) const;
   float getVoxel(QVector3D &position) const;
   float getVoxelInterpolated(double x, double y, double z) const;
   void setVoxel(int pos, float value);
   void setVoxel(int x, int y, int z, float value);
   void setVoxel(int x, int y, int z, int component, float value);

   CVolume_ resize(float sx, float sy, float sz, VISc::EInterpolation interpolationMode);

   long size() const { return (m_width * m_height * m_depth); }
   long totalVoxels() const { return (size() * m_numComponents); }
   long sizeInBytes() const { return (totalVoxels() * sizeof(float)); }
   int getWidth() const { return m_width; }
   int getHeight() const { return m_height; }
   int getDepth() const { return m_depth; }
   int getNumberOfComponents() const { return m_numComponents; }
   QColor getColor() const { return m_qcColor; }
   void setColor(QColor color) { m_qcColor = color; }
   QVector4D getPosition() const { return m_position; }
   void setPosition(QVector4D position) { m_position = position; }
   const float *getData() const { return m_data.data(); }
   float *getData() { return m_data.data(); }

protected:
   int coordinatesToPosition(int x, int y, int z) const
   {
      return ((x + (y * m_width) + (z * m_width * m_height)) * m_numComponents);
   }

   int m_width;
   int m_height;
   int m_depth;
   int m_numComponents;
   QColor m_qcColor;
   double m_aspectRatio[3];
   QVector4D m_position;
   QScopedArrayPointer<float> m_data;
};

#endif
