#ifndef CTRANSFERWIDGET_H
#define CTRANSFERWIDGET_H

//#include "visc.h"

#include <QtGui>
#include <QRadioButton>

#include "../CBasicTransferWidget.h"
#include "CTransferFunction.h"

#if defined(WIN32) && defined(_DEBUG)
#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif

class CTransferWidget : public CBasicTransferWidget
{
   Q_OBJECT

public:
   CTransferWidget(void);
   ~CTransferWidget(void);

   // Inherited methods
   void render() 
   { 
      //if (tmpVolumeList->size() < 1)
      //	return;

      //glPushAttrib(GL_ALL_ATTRIB_BITS);

      //float d = 4.0;
      //float r = d / 2.0;

      //for (int i = 0; i <= (tmpVolumeList->at(0)->getWidth() / d); i++)
      //	for (int j = 0; (j*1.73205*r) <= tmpVolumeList->at(0)->getHeight(); j++ )
      //		for (int k = 0; (k*r*2.44949*0.666666667) <= tmpVolumeList->at(0)->getDepth(); k++)
      //		{
      //			float x, y, z;
      //			x = y = z = 0.0;

      //			if ((k % 2) == 0)
      //			{
      //				x = i*2*r + r * (j % 2);
      //				y = 1.73205*r*j;
      //			}
      //			else
      //			{
      //				x = i*2*r-r*(j%2)+r;
      //				y = 1.73205*r*j-r;
      //			}
      //			z = 0.6666666667*2.44949*r*k;

      //			glColor3f(255, 0, 0);
      //			glPushMatrix();
      //				glTranslatef(-(tmpVolumeList->at(0)->getWidth()  / 2.0 /*0.8325*2*/) + x /*/ 2.0*/, 
      //							 -(tmpVolumeList->at(0)->getHeight() / 2.0 ) + y /*/ 2.0*/, 
      //							 -(tmpVolumeList->at(0)->getDepth()  / 2.0 ) + z /*/ 2.0*/);						
      //		
      //				glutWireSphere(r, 4, 4);
      //				//gluSphere(myQuad, r, 8, 8);
      //			glPopMatrix();
      //		}
      //glPopAttrib();
   };
   int load(QString fileName);
   int save(QString fileName);
   void zero();
   void reset();
   void clear() { tf->clear(); } ;
   QString getExtension();
   QString getDescription();
   void setContextMenu(QMenu *);
   int getWidth();
   QImage getTransferFunction(int);
   void computeHistogram(int, bool);
   bool isChanged() { return this->tf->isChanged(); };
   void setUnchanged() { this->tf->setUnchanged(); };
   bool isDirtyVolume() { return false; };
   void setCleanVolume() { };

   private slots:
      void hSpacingValueChanged(int);
      void vSpacingValueChanged(int);
      void radioButtonClicked(bool);

private:
   CTransferFunction *tf;

   // Interface
   QRadioButton *red, *green, *blue, *opacity;
   QSpinBox *hSpacing, *vSpacing;
};
#endif
