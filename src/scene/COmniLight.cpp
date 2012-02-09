#include "COmniLight.h"
#include "COmniLightProperties.h"

COmniLight::COmniLight() :
CLight(VISc::ltOmnilight),
   m_qcColor(QColor(255, 255, 255)),
   m_intensity(1.0)
{
}

void COmniLight::render()
{
   glPushMatrix();
   glColor4f(m_qcColor.redF(), m_qcColor.greenF(), m_qcColor.blueF(), 1.0);
   glTranslatef(m_qvPosition.getX(), m_qvPosition.getY(), m_qvPosition.getZ());

   GLUquadric *myQuad = gluNewQuadric();
   gluSphere(myQuad, m_radius * 1, 8, 8);
   glPopMatrix();
}

void COmniLight::properties()
{
   COmniLightProperties properties(this);
   properties.exec();
}

void COmniLight::getBoundingBox(double &mw, double &pw, double &mh, double &ph, double &md, double &pd)
{
   mw = m_qvPosition.getX() - m_radius;
   pw = m_qvPosition.getX() + m_radius;
   mh = m_qvPosition.getY() - m_radius;
   ph = m_qvPosition.getY() + m_radius;
   md = m_qvPosition.getZ() - m_radius;
   pd = m_qvPosition.getZ() + m_radius;
}

void COmniLight::setUpLight(int lightNumber)
{
   // Light position
   GLfloat lp[4]; 
   lp[0] = m_qvPosition.getX(); 
   lp[1] = m_qvPosition.getY(); 
   lp[2] = m_qvPosition.getZ(); 
   lp[3] = 1.0;
   glLightfv(GL_LIGHT0 + lightNumber, GL_POSITION, lp);

   // Diffuse and specular componentes
   GLfloat lc[4];
   lc[0] = m_qcColor.redF() * m_intensity;
   lc[1] = m_qcColor.greenF() * m_intensity;
   lc[2] = m_qcColor.blueF() * m_intensity;
   lc[3] = 1.0;

   glLightfv(GL_LIGHT0 + lightNumber, GL_DIFFUSE, lc);
   glLightfv(GL_LIGHT0 + lightNumber, GL_SPECULAR, lc);
   glLightfv(GL_LIGHT0 + lightNumber, GL_AMBIENT, lc);
}
