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

#include "COpenGLProperties.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

// General Tab =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
CTabGeneral::CTabGeneral(COpenGL *view)
{
	setWindowTitle(tr("Scene Properties"));
	tmpView = view;

	QVBoxLayout *vLayout = new QVBoxLayout(this);
	perspectiveGroup = new QGroupBox(tr("Perspective"));

	QHBoxLayout *pLayout = new QHBoxLayout();
	projectionLabel = new QLabel(tr("Projection Mode:"));
	projectionCombo = new QComboBox();
	projectionCombo->addItem(tr("Parallel"));
	projectionCombo->addItem(tr("Perspective"));
	if (tmpView->getProjectionMode() == VISc::pParallel)
	{
		perspectiveGroup->setEnabled(false);
		projectionCombo->setCurrentIndex(0);
	}
	else if (tmpView->getProjectionMode() == VISc::pPerspective)
	{	
		perspectiveGroup->setEnabled(true);
		projectionCombo->setCurrentIndex(1);
	}
	connect(projectionCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(projectionCurrentIndexChanged(int)));
	pLayout->addWidget(projectionLabel);
	pLayout->addWidget(projectionCombo);
	vLayout->addLayout(pLayout);

	eyeDistanceLabel = new QLabel(tr("Eye distance:"));
	eyeDistanceSpinbox = new QDoubleSpinBox();
	eyeDistanceSpinbox->setMinimum(110.0);
	eyeDistanceSpinbox->setMaximum(5000.0);
	eyeDistanceSpinbox->setValue( view->getEyeScreen() );
	connect(eyeDistanceSpinbox, SIGNAL(valueChanged(double)), this, SLOT(eyeDistanceSpinboxAction(double)));
	QHBoxLayout *perspLayout = new QHBoxLayout();
	perspLayout->addWidget(eyeDistanceLabel);
	perspLayout->addWidget(eyeDistanceSpinbox);
	perspectiveGroup->setLayout(perspLayout);
	vLayout->addWidget(perspectiveGroup);

	QHBoxLayout *tLayout = new QHBoxLayout();
	gradientThresholdLabel = new QLabel(tr("Gradient Threshold:"));
	gradientThresholdSpinbox = new QDoubleSpinBox();
	gradientThresholdSpinbox->setMinimum(0.0);
	gradientThresholdSpinbox->setMaximum(1.0);
	gradientThresholdSpinbox->setSingleStep(0.01);
	gradientThresholdSpinbox->setValue( view->getThreshold() );
	connect(gradientThresholdSpinbox, SIGNAL(valueChanged(double)), this, SLOT(gradientThresholdValueChanged(double)));
	tLayout->addWidget(gradientThresholdLabel);
	tLayout->addWidget(gradientThresholdSpinbox);
	vLayout->addLayout(tLayout);

	QHBoxLayout *shLayout = new QHBoxLayout();
	shininessLabel = new QLabel(tr("Shininess:"));
	shininessSpinbox = new QDoubleSpinBox();
	shininessSpinbox->setMinimum(0.0);
	shininessSpinbox->setMaximum(1000.0);
	shininessSpinbox->setValue( view->getShininess() );
	connect(shininessSpinbox, SIGNAL(valueChanged(double)), this, SLOT(shininessValueChanged(double)));
	shLayout->addWidget(shininessLabel);
	shLayout->addWidget(shininessSpinbox);
	vLayout->addLayout(shLayout);

	QVBoxLayout *visLayout = new QVBoxLayout();
	QGroupBox *visibilityBox = new QGroupBox(tr("Visibility"));
	boundingBoxCheckbox = new QCheckBox(tr("Visible Bounding Box"));
	boundingBoxCheckbox->setChecked( view->getDrawBoundingBox() );
	connect(boundingBoxCheckbox, SIGNAL(stateChanged(int)), this, SLOT(boundingBoxStateChanged(int)));
	lightsCheckbox = new QCheckBox(tr("Visible Lights"));
	lightsCheckbox->setChecked( view->getDrawLights() );
	connect(lightsCheckbox, SIGNAL(stateChanged(int)), this, SLOT(visibleLight(int)));
	visLayout->addWidget(boundingBoxCheckbox);
	visLayout->addWidget(lightsCheckbox);
	visibilityBox->setLayout(visLayout);
	vLayout->addWidget(visibilityBox);

	QHBoxLayout *cLayout = new QHBoxLayout();
	colorLabel = new QLabel(tr("Background Color:"));
	colorShower = new QLineEdit();
	colorShower->setEnabled(false);
	QPalette palette( colorShower->palette() );
	palette.setColor( QPalette::Base, view->getBackgroundColor() );
	colorShower->setPalette(palette);
	tmpColor = view->getBackgroundColor();

	colorPicker = new QPushButton(tr("Color"));
	connect(colorPicker, SIGNAL(clicked()), this, SLOT(colorPickerAction()));
	cLayout->addWidget(colorLabel);
	cLayout->addWidget(colorShower);
	cLayout->addWidget(colorPicker);
	vLayout->addLayout(cLayout);

	QGroupBox *zoomGroup = new QGroupBox(tr("Zoom tool"));
	QVBoxLayout *zoomLayout = new QVBoxLayout();
	zoomGroup->setLayout( zoomLayout );

	QHBoxLayout *zs = new QHBoxLayout();
	QLabel *zoomStyleLabel = new QLabel(tr("Rectangle Style:"));
	zoomStyleCombo = new QComboBox();
	zoomStyleCombo->insertItem(0, QIcon(":/resource/zoomStyleDashed.png"), tr("Dashed"));
	if (view->getZoomStyle() == 0xAAAA)
		zoomStyleCombo->setCurrentIndex(0);
	connect(zoomStyleCombo, SIGNAL(currentIndexChanged ( const QString & )), this, SLOT(currentIndexChangedZoomStyle ( const QString & )));
	zs->addWidget(zoomStyleLabel);
	zs->addWidget(zoomStyleCombo);
	zoomLayout->addLayout(zs);

	QLabel *zoomFactor = new QLabel(tr("Scale:"));
	QHBoxLayout *zf = new QHBoxLayout();
	zoomFactorSpin = new QSpinBox();
	zoomFactorSpin->setValue( view->getZoomFactor() );
	zoomFactorSpin->setMinimum(1);
	zoomFactorSpin->setMaximum(10);
	connect(zoomFactorSpin, SIGNAL(valueChanged(int)), this, SLOT(zoomFactorSpinboxAction(int)));
	zf->addWidget(zoomFactor);
	zf->addWidget(zoomFactorSpin);
	zoomLayout->addLayout(zf);

	QLabel *zoomWidth = new QLabel(tr("Rectangle Width:"));
	QHBoxLayout *zw = new QHBoxLayout();
	zoomWidthSpin = new QSpinBox();
	zoomWidthSpin->setValue( view->getZoomWidth() );
	zoomWidthSpin->setMinimum(1);
	zoomWidthSpin->setMaximum(10);
	connect(zoomWidthSpin, SIGNAL(valueChanged(int)), this, SLOT(zoomWidthSpinboxAction(int)));
	zw->addWidget(zoomWidth);
	zw->addWidget(zoomWidthSpin);
	zoomLayout->addLayout(zw);

	QHBoxLayout *zc = new QHBoxLayout();
	QLabel *zoomColor = new QLabel(tr("Rectangle Color:"));
	zoomColorShower = new QLineEdit();
	zoomColorShower->setEnabled(false);
	QPalette paletteZoom( zoomColorShower->palette() );
	paletteZoom.setColor( QPalette::Base, view->getZoomColor() );
	zoomColorShower->setPalette(paletteZoom);
	tmpColorZoom = view->getZoomColor();

	zoomColorPicker = new QPushButton(tr("Color"));
	connect(zoomColorPicker, SIGNAL(clicked()), this, SLOT(zoomColorPickerAction()));
	zc->addWidget(zoomColor);
	zc->addWidget(zoomColorShower);
	zc->addWidget(zoomColorPicker);
	zoomLayout->addLayout(zc);


	vLayout->addWidget(zoomGroup);

	vLayout->addStretch(1);
	this->update();
}

void CTabGeneral::gradientThresholdValueChanged(double value)
{
	tmpView->setThreshold(value);
}

void CTabGeneral::shininessValueChanged(double value)
{
	tmpView->setShininess(value);
}

void CTabGeneral::boundingBoxStateChanged(int)
{
	tmpView->setDrawBoundingBox( boundingBoxCheckbox->isChecked() );
}

void CTabGeneral::lightsStateChanged(int)
{
	tmpView->setDrawLights( lightsCheckbox->isChecked() );
}

void CTabGeneral::colorPickerAction()
{
	tmpColor = QColorDialog::getColor(Qt::white, this);
	tmpView->setBackgroundColor( tmpColor );
	QPalette palette( colorShower->palette() );
	palette.setColor( QPalette::Base, tmpColor );
	colorShower->setPalette(palette);
}

void CTabGeneral::projectionCurrentIndexChanged(int value)
{
	if (value == 0) // Parallel
	{
		perspectiveGroup->setEnabled(false);
		tmpView->setProjectionMode(VISc::pParallel);
	}
	else if (value == 1) // Perspective
	{
		perspectiveGroup->setEnabled(true);
		tmpView->setProjectionMode(VISc::pPerspective);
	}
}
void CTabGeneral::eyeDistanceSpinboxAction(double value)
{
	tmpView->setEyeScreen( value );
}
void CTabGeneral::update()
{
	if (tmpView->getProjectionMode() == VISc::pParallel)
	{
		perspectiveGroup->setEnabled(false);
		projectionCombo->setCurrentIndex(0);
	}
	else if (tmpView->getProjectionMode() == VISc::pPerspective)
	{	
		perspectiveGroup->setEnabled(true);
		projectionCombo->setCurrentIndex(1);
	}
	eyeDistanceSpinbox->setValue( tmpView->getEyeScreen() );
	gradientThresholdSpinbox->setValue( tmpView->getThreshold() );
	shininessSpinbox->setValue( tmpView->getShininess() );
	boundingBoxCheckbox->setChecked( tmpView->getDrawBoundingBox() );
	lightsCheckbox->setChecked( tmpView->getDrawLights() );

	QPalette palette( colorShower->palette() );
	palette.setColor( QPalette::Base, tmpView->getBackgroundColor() );
	colorShower->setPalette(palette);
	tmpColor = tmpView->getBackgroundColor();

}
void CTabGeneral::currentIndexChangedZoomStyle ( const QString & text )
{
	if (text == "Dashed")
		tmpView->setZoomStyle(0xAAAA);
}

void CTabGeneral::zoomFactorSpinboxAction(int value)
{
	tmpView->setZoomFactor(value);
}
void CTabGeneral::zoomWidthSpinboxAction(int value)
{
	tmpView->setZoomWidth(value);
}

void CTabGeneral::zoomColorPickerAction()
{
	tmpColorZoom = QColorDialog::getColor(Qt::white, this);
	tmpView->setZoomColor( tmpColorZoom );
	QPalette palette( zoomColorShower->palette() );
	palette.setColor( QPalette::Base, tmpColorZoom );
	zoomColorShower->setPalette(palette);
}
void CTabGeneral::visibleLight(int)
{
}
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// Wireframe Tab =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
CTabWireframe::CTabWireframe(COpenGL *view)
{
	tmpView = view;

	QVBoxLayout *vLayout = new QVBoxLayout(this);
	QLabel *tmpLabel = new QLabel(tr("No options available"));
	vLayout->addWidget(tmpLabel);
}
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// Unreal Time Tab =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
CTabUnrealTime::CTabUnrealTime(COpenGL *view)
{
	tmpView = view;

	QVBoxLayout *vLayout = new QVBoxLayout(this);

	QHBoxLayout *sLayout = new QHBoxLayout();
	stepDistanceLabel = new QLabel(tr("Step Distance:"));
	stepDistanceSpinbox = new QSpinBox();
	connect(stepDistanceSpinbox, SIGNAL(valueChanged(int)), this, SLOT(stepDistanceValueChanged(int)));
	sLayout->addWidget(stepDistanceLabel);
	sLayout->addWidget(stepDistanceSpinbox);
	vLayout->addLayout(sLayout);

	vLayout->addStretch(1);
}

void CTabUnrealTime::stepDistanceValueChanged(int)
{
}
void CTabUnrealTime::update()
{
}
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// Real Time Tab =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
CTabRealTime::CTabRealTime(COpenGL *view)
{
	tmpView = view;

	QVBoxLayout *vLayout = new QVBoxLayout(this);

	QVBoxLayout *vvLayout = new QVBoxLayout();
	viewingVectorGroup = new QGroupBox(tr("Viewing Vector"));
	viewingVectorCheckbox = new QCheckBox(tr("Visible"));
	viewingVectorCheckbox->setChecked( tmpView->getDisplayViewingVector() );
	connect(viewingVectorCheckbox, SIGNAL(stateChanged(int)), this, SLOT(viewVectorVisible(int)));
	vvLayout->addWidget(viewingVectorCheckbox);

	QHBoxLayout *cLayout = new QHBoxLayout();
	viewingVectorColorLabel = new QLabel(tr("Text color:"));
	viewingVectorColorShower = new QLineEdit();
	viewingVectorColorShower->setEnabled(false);
	QPalette palette( viewingVectorColorShower->palette() );
	palette.setColor( QPalette::Base, view->getViewingVectorColor() );
	viewingVectorColorShower->setPalette(palette);
	tmpColor = view->getViewingVectorColor();

	viewingVectorColorPicker = new QPushButton(tr("Color"));
	connect(viewingVectorColorPicker, SIGNAL(clicked()), this, SLOT(viewingVectorColorPickerAction()));
	cLayout->addWidget(viewingVectorColorLabel);
	cLayout->addWidget(viewingVectorColorShower);
	cLayout->addWidget(viewingVectorColorPicker);
	vvLayout->addLayout(cLayout);
	viewingVectorGroup->setLayout(vvLayout);
	vLayout->addWidget(viewingVectorGroup);

	QHBoxLayout *sLayout = new QHBoxLayout();
	sliceDistanceLabel = new QLabel(tr("Slice Number:"));
	sliceDistanceSpinbox = new QSpinBox();
	sliceDistanceSpinbox->setMinimum(1.0);
	sliceDistanceSpinbox->setMaximum(10.0);
	sliceDistanceSpinbox->setValue( view->getSliceNumber() );
	connect(sliceDistanceSpinbox, SIGNAL(valueChanged(int)), this, SLOT(sliceDistanceValueChanged(int)));
	sLayout->addWidget(sliceDistanceLabel);
	sLayout->addWidget(sliceDistanceSpinbox);
	vLayout->addLayout(sLayout);

	vLayout->addStretch(1);
}

void CTabRealTime::sliceDistanceValueChanged(int value)
{
	tmpView->setSliceNumber( value );
}

void CTabRealTime::viewingVectorColorPickerAction()
{
	tmpColor = QColorDialog::getColor(Qt::white, this);
	tmpView->setViewingVectorColor( tmpColor );
	QPalette palette( viewingVectorColorShower->palette() );
	palette.setColor( QPalette::Base, tmpColor );
	viewingVectorColorShower->setPalette(palette);
}
void CTabRealTime::update()
{
}
void CTabRealTime::viewVectorVisible(int)
{
	if (viewingVectorCheckbox->isChecked())
		tmpView->setDisplayViewingVector(true);
	else
		tmpView->setDisplayViewingVector(false);
}
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// OpenGL Properties Tab =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
COpenGLProperties::COpenGLProperties(COpenGL *view)
{
	tmpView = view;

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainTab = new QTabWidget();
	
	generalTab = new CTabGeneral(view);
	//wireTab = new CTabWireframe(view);
	unrealTab = new CTabUnrealTime(view);
	realTab = new CTabRealTime(view);

	mainTab->addTab(generalTab, tr("General"));
	//mainTab->addTab(wireTab, tr("Wireframe"));
	mainTab->addTab(realTab, tr("2D Texture Rendering"));
	mainTab->addTab(unrealTab, tr("Direct Volume Rendering"));

	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accepted()));

	mainLayout->addWidget(mainTab);
	mainLayout->addWidget(buttonBox);
}

COpenGLProperties::~COpenGLProperties(void)
{
}

void COpenGLProperties::accepted()
{
	this->close();
}
void COpenGLProperties::update()
{
	generalTab->update();
	unrealTab->update();
	realTab->update();
}
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
