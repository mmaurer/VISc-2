#ifndef CLIGHT_H
#define CLIGHT_H

#include "../visc.h"

#include <QtGui>
#include <QString>

#if defined(WIN32) && defined(_DEBUG)
#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif

class CLight
{
public:
   CLight(VISc::ELightType lightType);
   virtual ~CLight() { }

   virtual void render() { }
   virtual void properties();
   virtual void getBoundingBox(double &, double &, double &, double &, double &, double &) { }
   virtual void setUpLight(int) { }

   VISc::ELightType getType() { return m_lightType; } const
   void setType (VISc::ELightType value) { m_lightType = value; }
   void setName(QString value) { m_name = value; }
   QString getName() { return m_name; } const
   void setEnable(bool e) { m_bEnabled = e; }
   bool getEnable() { return m_bEnabled; } const
   QDateTime getTimestamp() { return m_timeStamp; } const
   void setTimestamp(QDateTime value) { m_timeStamp = value; }

protected:
   bool m_bEnabled;
   VISc::ELightType m_lightType;
   QString m_name;
   QDateTime m_timeStamp;
};

#endif
