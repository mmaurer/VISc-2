#ifndef COMNILIGHTPROPERTIES_H
#define COMNILIGHTPROPERTIES_H

#include <QDialog>

#include "COmniLight.h"

class COmniLight;

#if defined(WIN32) && defined(_DEBUG)
   #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
   #define new DEBUG_NEW
#endif

class COmniLightProperties : public QDialog
{
	Q_OBJECT

	public:
		COmniLightProperties(COmniLight *);
		~COmniLightProperties(void);

	private slots:
		void okAction();
		void cancelAction();
		void colorPickerAction();

	private:
		COmniLight *tmpLight;
		QColor tmpColor;

		QLabel *nameLabel;
		QLineEdit *name;

		QGroupBox *positionBox;
		QLabel *x, *y, *z;
		QDoubleSpinBox *xv, *yv, *zv;
		
		QLabel *colorLabel;
		QLineEdit *colorShower;
		QColorDialog *colorDialog;
		QPushButton *colorPicker;

		QLabel *intensityLabel;
		QDoubleSpinBox *intensity;

		QPushButton *ok, *cancel;
};

#endif
