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
