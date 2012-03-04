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

#include "CTransferWidget.h"

#include <QHBoxLayout>

CTransferWidget::CTransferWidget(void) :
CBasicTransferWidget(tr("1tf"), tr("1D Transfer Function"), VISc::twoLoad | VISc::twoSave | VISc::twoZero | VISc::twoReset, Qt::RightDockWidgetArea)
{
	tf = new CTransferFunction();
	tf->setMinimumHeight(tf->getHeight());
	tf->setMaximumHeight(tf->getHeight());
	this->setMinimumWidth(tf->getWidth());
	this->setMaximumWidth(tf->getWidth());

	// Radio buttons
	red = new QRadioButton(tr("Red"));
	connect(red, SIGNAL(clicked(bool)), this, SLOT(radioButtonClicked(bool)));
	green = new QRadioButton(tr("Green"));
	connect(green, SIGNAL(clicked(bool)), this, SLOT(radioButtonClicked(bool)));
	blue = new QRadioButton(tr("Blue"));
	connect(blue, SIGNAL(clicked(bool)), this, SLOT(radioButtonClicked(bool)));
	opacity = new QRadioButton(tr("Opacity"));
	opacity->setChecked(true);
	connect(opacity, SIGNAL(clicked(bool)), this, SLOT(radioButtonClicked(bool)));

	QHBoxLayout *radioLayout = new QHBoxLayout();
	radioLayout->addWidget(red);
	radioLayout->addWidget(green);
	radioLayout->addWidget(blue);
	radioLayout->addWidget(opacity);

	// Grid spacing
	QHBoxLayout *spacingLayout = new QHBoxLayout();
	QVBoxLayout *hsLayout = new QVBoxLayout();
	QVBoxLayout *vsLayout = new QVBoxLayout();
	QLabel *horizontalSpacing = new QLabel(tr("Horizontal grid spacing"));
	QLabel *verticalSpacing = new QLabel(tr("Vertical grid spacing"));
	hSpacing = new QSpinBox();
	hSpacing->setValue(tf->getGridX());
	hSpacing->setMinimum(5);
	connect(hSpacing, SIGNAL(valueChanged(int)), this, SLOT(hSpacingValueChanged(int)));
	vSpacing = new QSpinBox();
	vSpacing->setValue(tf->getGridY());
	vSpacing->setMinimum(5);
	connect(vSpacing, SIGNAL(valueChanged(int)), this, SLOT(vSpacingValueChanged(int)));
	
	hsLayout->addWidget(horizontalSpacing);
	hsLayout->addWidget(hSpacing);
	vsLayout->addWidget(verticalSpacing);
	vsLayout->addWidget(vSpacing);
	
	spacingLayout->addLayout(hsLayout);
	spacingLayout->addLayout(vsLayout);


	// Final assembly of the right panel
	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->setMargin(0);
	mainLayout->addWidget(tf);
	mainLayout->addLayout(radioLayout);
	mainLayout->addLayout(spacingLayout);
	mainLayout->addStretch(1);

	this->setLayout(mainLayout);
}

CTransferWidget::~CTransferWidget(void)
{
	delete tf;
}

// Inherited methods
int CTransferWidget::load(QString fileName)
{
	tf->setChanged();
	return tf->load(fileName);
}

int CTransferWidget::save(QString fileName)
{
	return tf->save(fileName);	
}

void CTransferWidget::zero()
{
	tf->setChanged();
	tf->zero();
}

void CTransferWidget::reset()
{
	tf->setChanged();
	tf->reset();
}
QString CTransferWidget::getExtension()
{
	return "1tf";
}
QString CTransferWidget::getDescription()
{
	return tr("1D Transfer Function");
}
void CTransferWidget::setContextMenu(QMenu *menu)
{ 
	m_contextMenuPtr = menu;
	tf->setContextMenu(menu);
}
int CTransferWidget::getWidth()
{
	return tf->getWidth();
}
QImage CTransferWidget::getTransferFunction(int)
{
	return tf->getTransferFunction();
}
void CTransferWidget::computeHistogram(int volumeIndex, bool resetTransferFunction)
{
	if ((m_volumeListPtr->count() > 0) && (volumeIndex < m_volumeListPtr->count()))
		tf->computeHistogram(*(m_volumeListPtr->at(volumeIndex)), resetTransferFunction);
}
//end of inherited members

// Slots
void CTransferWidget::hSpacingValueChanged(int value)
{
	tf->setGridX(value);
}
void CTransferWidget::vSpacingValueChanged(int value)
{
	tf->setGridY(value);
}
void CTransferWidget::radioButtonClicked(bool)
{
	if (red->isChecked())
		tf->setComponent(VISc::cRed);
	else if (green->isChecked())
		tf->setComponent(VISc::cGreen);
	else if (blue->isChecked())
		tf->setComponent(VISc::cBlue);
	else if (opacity->isChecked())
		tf->setComponent(VISc::cOpacity);
}
// end of slots
