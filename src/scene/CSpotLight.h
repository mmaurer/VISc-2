#ifndef CSPOTLIGHT_H
#define CSPOTLIGHT_H

#include "../visc.h"
#include "CLight.h"

#include "../algebra/MathUtils.h"
#include "../algebra/CVector.h"

#include <QtGui>

#if defined(WIN32) && defined(_DEBUG)
#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif

class CSpotLight : public CLight
{
public:
   CSpotLight();
   virtual ~CSpotLight() { }

   virtual void render() { }
   virtual void properties() { }

private:
};

#endif
