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

#ifndef COMNILIGHT_H
#define COMNILIGHT_H

#include <GL/glew.h>     //--Extension Library-->Always include this before  <gl.h>
#include <GL/gl.h>

#include <QtGui>
#include <QtOpenGL/QGLWidget>

#include "../algebra/MathUtils.h"
#include "../algebra/CVector.h"

#include "CLight.h"

#if defined(WIN32) && defined(_DEBUG)
#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif

class COmniLight : public CLight
{
public:
   COmniLight();
   COmniLight(const COmniLight &other);
   virtual ~COmniLight() { }

   virtual void render();
   virtual void properties();
   virtual void getBoundingBox(float &mw, float &pw, float &mh, float &ph, float &md, float &pd);
   virtual void setUpLight(int);

   COmniLight &operator=(const COmniLight &other)
   {
      if (this != &other)
      {
         m_qvPosition = other.m_qvPosition;
         m_qcColor = other.m_qcColor;
         m_intensity = other.m_intensity;

         CLight::copyFrom(other);
         CSceneObject::copyFrom(other);
      }
      return *this;
   }

   CVector getPosition() { return m_qvPosition; } const
   void setPosition(CVector value) { m_qvPosition = value; }
   QColor getColor() { return m_qcColor; } const
   void setColor(QColor value) { m_qcColor = value; }
   float getIntensity() { return m_intensity; } const
   void setIntensity(float value) { m_intensity = value; }

protected:
   CVector m_qvPosition;
   QColor  m_qcColor;
   float   m_intensity;
   static const int m_radius = 1;
};

#endif
