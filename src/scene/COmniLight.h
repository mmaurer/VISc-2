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
   virtual ~COmniLight() { }

   virtual void render();
   virtual void properties();
   virtual void getBoundingBox(double &mw, double &pw, double &mh, double &ph, double &md, double &pd);
   virtual void setUpLight(int);

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
