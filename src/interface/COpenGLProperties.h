#ifndef COPENGLPROPERTIES_H
#define COPENGLPROPERTIES_H

#include <QDialog>
#include <QWidget>

#include "../visc.h"
#include "../interface/COpenGL.h"

#if defined(WIN32) && defined(_DEBUG)
   #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
   #define new DEBUG_NEW
#endif

class CTabGeneral : public QWidget
{
	Q_OBJECT
public:
	CTabGeneral(COpenGL *);

	void update();
private:
	COpenGL *tmpView;
	QColor tmpColor;
	QColor tmpColorZoom;

	QLabel *projectionLabel;
	QComboBox *projectionCombo;

	QGroupBox *perspectiveGroup;
	QLabel *eyeDistanceLabel;
	QDoubleSpinBox *eyeDistanceSpinbox;

	QLabel *gradientThresholdLabel;
	QDoubleSpinBox *gradientThresholdSpinbox;

	QLabel *shininessLabel;
	QDoubleSpinBox *shininessSpinbox;

	QCheckBox *boundingBoxCheckbox;
	QCheckBox *lightsCheckbox;

	QLabel *colorLabel;
	QLineEdit *colorShower;
	QPushButton *colorPicker;

	QComboBox *zoomStyleCombo;
	QSpinBox *zoomWidthSpin;
	QSpinBox *zoomFactorSpin;
	QLineEdit *zoomColorShower;
	QPushButton *zoomColorPicker;

private slots:
	void gradientThresholdValueChanged(double);
	void shininessValueChanged(double);
	void visibleLight(int);
	void boundingBoxStateChanged(int);
	void lightsStateChanged(int);
	void colorPickerAction();
	void projectionCurrentIndexChanged(int);
	void eyeDistanceSpinboxAction(double);
	void currentIndexChangedZoomStyle ( const QString & );
	void zoomFactorSpinboxAction(int);
	void zoomWidthSpinboxAction(int);
	void zoomColorPickerAction();
};

class CTabWireframe : public QWidget
{
	Q_OBJECT
public:
	CTabWireframe(COpenGL *);
	void update();
private:
	COpenGL *tmpView;
private slots:
};

class CTabUnrealTime : public QWidget
{
	Q_OBJECT
public:
	CTabUnrealTime(COpenGL *);
	void update();
private:
	COpenGL *tmpView;

	QLabel *stepDistanceLabel;
	QSpinBox *stepDistanceSpinbox;
private slots:
	void stepDistanceValueChanged(int);
};

class CTabRealTime : public QWidget
{
	Q_OBJECT
public:
	CTabRealTime(COpenGL *);
	void update();
private:
	COpenGL *tmpView;
	QColor tmpColor;

	QLabel *sliceDistanceLabel;
	QSpinBox *sliceDistanceSpinbox;

	QGroupBox *viewingVectorGroup;
	QCheckBox *viewingVectorCheckbox;
	QLabel *viewingVectorColorLabel;
	QLineEdit *viewingVectorColorShower;
	QPushButton *viewingVectorColorPicker;
private slots:
	void sliceDistanceValueChanged(int);
	void viewingVectorColorPickerAction();
	void viewVectorVisible(int);
};


class COpenGLProperties : public QDialog
{
	Q_OBJECT

	public:
		COpenGLProperties(COpenGL *);
		~COpenGLProperties(void);

		void update();

	private:
		COpenGL *tmpView;
		CTabGeneral *generalTab;
		CTabWireframe *wireTab;
		CTabUnrealTime *unrealTab;
		CTabRealTime *realTab;

		// Layout
		QTabWidget *mainTab;
		QDialogButtonBox *buttonBox;

	private slots:
		void accepted();
};

#endif
