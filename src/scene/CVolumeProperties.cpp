#include "CVolumeProperties.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QBoxLayout>

CVolumeProperties::CVolumeProperties(CVolume *volume)
{
	setWindowTitle(tr("Volume Properties"));
	tmpVolume = volume;
	
	QVBoxLayout *vLayout = new QVBoxLayout(this);

	nameLabel = new QLabel(tr("Name of volume"));
	name = new QLineEdit();
	name->setText(volume->getName());
	vLayout->addWidget(nameLabel);
	vLayout->addWidget(name);

	QHBoxLayout *dLayout = new QHBoxLayout();
	dimensionsBox = new QGroupBox(tr("Dimension of volume"));
	dimensionsLabel = new QLabel(tr("%1 x %2 x %3").arg(volume->getWidth()).arg(volume->getHeight()).arg(volume->getDepth()));
	dLayout->addWidget(dimensionsLabel);
	dimensionsBox->setLayout(dLayout);

	vLayout->addWidget(dimensionsBox);

	QHBoxLayout *pLayout = new QHBoxLayout();
	positionBox = new QGroupBox(tr("Position"));
	x = new QLabel(tr("X:"));
	y = new QLabel(tr("Y:"));
	z = new QLabel(tr("Z:"));
	xv = new QDoubleSpinBox();
	xv->setMinimum(-1000);
	xv->setMaximum(1000);
	yv = new QDoubleSpinBox();
	yv->setMinimum(-1000);
	yv->setMaximum(1000);
	zv = new QDoubleSpinBox();
	zv->setMinimum(-1000);
	zv->setMaximum(1000);
	xv->setValue(volume->getPosition().getX());
	yv->setValue(volume->getPosition().getY());
	zv->setValue(volume->getPosition().getZ());
	pLayout->addWidget(x);
	pLayout->addWidget(xv);
	pLayout->addWidget(y);
	pLayout->addWidget(yv);
	pLayout->addWidget(z);
	pLayout->addWidget(zv);
	positionBox->setLayout(pLayout);

	vLayout->addWidget(positionBox);

	QHBoxLayout *arLayout = new QHBoxLayout();
	aspectRatioBox = new QGroupBox(tr("Aspect Ratio"));
	xr = new QLabel(tr("X:"));
	yr = new QLabel(tr("Y:"));
	zr = new QLabel(tr("Z:"));
	xvr = new QDoubleSpinBox();
	xvr->setMinimum(-10);
	xvr->setMaximum(10);
	yvr = new QDoubleSpinBox();
	yvr->setMinimum(-10);
	yvr->setMaximum(10);
	zvr = new QDoubleSpinBox();
	zvr->setMinimum(-10);
	zvr->setMaximum(10);
	xvr->setValue(volume->getAspectRatio(VISc::dX));
	yvr->setValue(volume->getAspectRatio(VISc::dY));
	zvr->setValue(volume->getAspectRatio(VISc::dZ));
	arLayout->addWidget(xr);
	arLayout->addWidget(xvr);
	arLayout->addWidget(yr);
	arLayout->addWidget(yvr);
	arLayout->addWidget(zr);
	arLayout->addWidget(zvr);
	aspectRatioBox->setLayout(arLayout);

	vLayout->addWidget(aspectRatioBox);

	QHBoxLayout *cLayout = new QHBoxLayout();
	colorLabel = new QLabel(tr("Color:"));
	colorShower = new QLineEdit();
	colorShower->setEnabled(false);
	QPalette palette( colorShower->palette() );
	palette.setColor( QPalette::Base, volume->getColor() );
	colorShower->setPalette(palette);
	tmpColor = volume->getColor();

	colorPicker = new QPushButton(tr("Color"));
	connect(colorPicker, SIGNAL(clicked()), this, SLOT(colorPickerAction()));
	cLayout->addWidget(colorLabel);
	cLayout->addWidget(colorShower);
	cLayout->addWidget(colorPicker);
	vLayout->addLayout(cLayout);

	//QHBoxLayout *iLayout = new QHBoxLayout();
	//intensityLabel = new QLabel(tr("Intensity"));
	//intensity = new QDoubleSpinBox();
	//intensity->setMinimum(0.0);
	//intensity->setMaximum(1.0);
	//intensity->setValue(light->getIntensity());
	//iLayout->addWidget(intensityLabel);
	//iLayout->addWidget(intensity);
	//intensity->setValue(light->getIntensity());

	//vLayout->addLayout(iLayout);

	QHBoxLayout *buttons = new QHBoxLayout();
	ok = new QPushButton(tr("OK"));
	ok->setDefault(true);
	connect(ok, SIGNAL(clicked()), this, SLOT(okAction()));
	cancel = new QPushButton(tr("Cancel"));
	connect(cancel, SIGNAL(clicked()), this, SLOT(cancelAction()));
	buttons->addWidget(ok);
	buttons->addWidget(cancel);

	vLayout->addLayout(buttons);
}

CVolumeProperties::~CVolumeProperties(void)
{
}

// Slots
void CVolumeProperties::okAction()
{
	if (!name->text().isEmpty())
		tmpVolume->setName(name->text());

	CVector position(xv->value(), yv->value(), zv->value());
	tmpVolume->setPosition(position);
	tmpVolume->setAspectRatio(xvr->value(), yvr->value(), zvr->value());
	tmpVolume->setColor(tmpColor);

	this->close();
}

void CVolumeProperties::cancelAction()
{
	this->close();
}

void CVolumeProperties::colorPickerAction()
{
	tmpColor = QColorDialog::getColor(Qt::white, this);
	QPalette palette( colorShower->palette() );
	palette.setColor( QPalette::Base, tmpColor );
	colorShower->setPalette(palette);
}
// end of slots
