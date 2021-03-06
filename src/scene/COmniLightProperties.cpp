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

#include "COmniLightProperties.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QBoxLayout>
#include <QPushButton>

COmniLightProperties::COmniLightProperties(COmniLight *light)
{
	setWindowTitle(tr("Omni Light Properties"));
	tmpLight = light;
	
	QVBoxLayout *vLayout = new QVBoxLayout(this);

	nameLabel = new QLabel(tr("Name of light"));
	name = new QLineEdit();
	name->setText(light->getName());
	vLayout->addWidget(nameLabel);
	vLayout->addWidget(name);

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
	xv->setValue(light->getPosition().getX());
	yv->setValue(light->getPosition().getY());
	zv->setValue(light->getPosition().getZ());
	pLayout->addWidget(x);
	pLayout->addWidget(xv);
	pLayout->addWidget(y);
	pLayout->addWidget(yv);
	pLayout->addWidget(z);
	pLayout->addWidget(zv);
	positionBox->setLayout(pLayout);

	vLayout->addWidget(positionBox);

	QHBoxLayout *cLayout = new QHBoxLayout();
	colorLabel = new QLabel(tr("Color:"));
	colorShower = new QLineEdit();
	colorShower->setEnabled(false);
	QPalette palette( colorShower->palette() );
	palette.setColor( QPalette::Base, light->getColor() );
	colorShower->setPalette(palette);
	tmpColor = light->getColor();

	colorPicker = new QPushButton(tr("Color"));
	connect(colorPicker, SIGNAL(clicked()), this, SLOT(colorPickerAction()));
	cLayout->addWidget(colorLabel);
	cLayout->addWidget(colorShower);
	cLayout->addWidget(colorPicker);
	vLayout->addLayout(cLayout);

	QHBoxLayout *iLayout = new QHBoxLayout();
	intensityLabel = new QLabel(tr("Intensity"));
	intensity = new QDoubleSpinBox();
	intensity->setMinimum(0.0);
	intensity->setMaximum(1.0);
	intensity->setDecimals(5);
	intensity->setValue(light->getIntensity());
	iLayout->addWidget(intensityLabel);
	iLayout->addWidget(intensity);
	intensity->setValue(light->getIntensity());

	vLayout->addLayout(iLayout);

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

COmniLightProperties::~COmniLightProperties(void)
{
}

// Slots
void COmniLightProperties::okAction()
{
	if (!name->text().isEmpty())
		tmpLight->setName(name->text());

	CVector position(xv->value(), yv->value(), zv->value());
	tmpLight->setPosition(position);
	tmpLight->setColor(tmpColor);
	tmpLight->setIntensity(intensity->value());

	this->close();
}

void COmniLightProperties::cancelAction()
{
	this->close();
}

void COmniLightProperties::colorPickerAction()
{
	tmpColor = QColorDialog::getColor(Qt::white, this);
	QPalette palette( colorShower->palette() );
	palette.setColor( QPalette::Base, tmpColor );
	colorShower->setPalette(palette);
}
// end of slots
