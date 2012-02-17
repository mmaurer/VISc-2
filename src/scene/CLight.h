#ifndef CLIGHT_H
#define CLIGHT_H

#include "../visc.h"
#include "CSceneObject.h"

#if defined(WIN32) && defined(_DEBUG)
   #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
   #define new DEBUG_NEW
#endif

class CLight : public CSceneObject
{
public:
   CLight(VISc::ELightType lightType) :
       m_lightType(lightType)
   {
   }

   virtual ~CLight() { }

   virtual void render() { }
   virtual void properties() { }
   virtual void getBoundingBox(float &, float &, float &, float &, float &, float &) { }
   virtual void setUpLight(int) { }

   VISc::ELightType getType() { return m_lightType; } const
   void setType (VISc::ELightType value) { m_lightType = value; }

protected:
   VISc::ELightType m_lightType;
};

#endif
