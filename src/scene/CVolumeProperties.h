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

#ifndef CVOLUMEPROPERTIES_H
#define CVOLUMEPROPERTIES_H

#include <QtGui>
#include <QDialog>
#include <QLabel>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QColorDialog>

//#include "CVolume.h"

class CVolume;

#if defined(WIN32) && defined(_DEBUG)
   #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
   #define new DEBUG_NEW
#endif

class CVolumeProperties : public QDialog
{
	Q_OBJECT

	public:
		CVolumeProperties(CVolume *);
		~CVolumeProperties(void);

	private slots:
		void okAction();
		void cancelAction();
		void colorPickerAction();

	private:
		CVolume *tmpVolume;
		QColor tmpColor;

		QLabel *nameLabel;
		QLineEdit *name;

		QGroupBox *dimensionsBox;
		QLabel *dimensionsLabel;

		QGroupBox *positionBox;
		QLabel *x, *y, *z;
		QDoubleSpinBox *xv, *yv, *zv;

		QGroupBox *aspectRatioBox;
		QLabel *xr, *yr, *zr;
		QDoubleSpinBox *xvr, *yvr, *zvr;

		QLabel *colorLabel;
		QLineEdit *colorShower;
		QColorDialog *colorDialog;
		QPushButton *colorPicker;

		//QLabel *intensityLabel;
		//QDoubleSpinBox *intensity;

		QPushButton *ok, *cancel;
};

#endif
