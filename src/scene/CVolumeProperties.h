#ifndef CVOLUMEPROPERTIES_H
#define CVOLUMEPROPERTIES_H

#include <QtGui>
#include <QDialog>
#include <QLabel>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QColorDialog>

#include "CVolume.h"

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
