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

#include "CScreenShot.h"

#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QFileDialog>
#include <QImageWriter>

CScreenShot::CScreenShot(QSize size)
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QVBoxLayout *sizeLayout = new QVBoxLayout();
	QGroupBox *sizeGroup = new QGroupBox(tr("Screenshot size"));
    m_qlWidthLabel = QSharedPointer<QLabel>(new QLabel(tr("Width")));
    m_qsbWidthSpin = QSharedPointer<QSpinBox>( new QSpinBox() );
    m_qsbWidthSpin->setMinimum(1);
    m_qsbWidthSpin->setMaximum(5000);
    m_qsbWidthSpin->setValue(size.width());
	
	QHBoxLayout *w = new QHBoxLayout();
    w->addWidget(m_qlWidthLabel.data());
    w->addWidget(m_qsbWidthSpin.data());

    m_qlHeightLabel = QSharedPointer<QLabel>(new QLabel(tr("Height")));
    m_qsbHeightSpin = QSharedPointer<QSpinBox>(new QSpinBox());
    m_qsbHeightSpin->setMinimum(1);
    m_qsbHeightSpin->setMaximum(5000);
    m_qsbHeightSpin->setValue(size.height());

	QHBoxLayout *h = new QHBoxLayout();
    h->addWidget(m_qlHeightLabel.data());
    h->addWidget(m_qsbHeightSpin.data());

	sizeLayout->addLayout(w);
	sizeLayout->addLayout(h);
	sizeGroup->setLayout(sizeLayout);
		
	mainLayout->addWidget(sizeGroup);

    QHBoxLayout fileNameLayout;
    //QHBoxLayout *fileNameLayout = new QHBoxLayout();
    m_qlFileNameLabel = QSharedPointer<QLabel>( new QLabel(tr("File name:")) );
    m_qleFileNameEdit = QSharedPointer<QLineEdit>( new QLineEdit() );
    m_qpbFileNameButton = QSharedPointer<QPushButton>( new QPushButton(tr("Browse")) );
    connect(m_qpbFileNameButton.data(), SIGNAL(clicked()), this, SLOT(clicked()));
    fileNameLayout.addWidget(m_qlFileNameLabel.data());
    fileNameLayout.addWidget(m_qleFileNameEdit.data());
    fileNameLayout.addWidget(m_qpbFileNameButton.data());
    mainLayout->addLayout(&fileNameLayout);

    m_qdbbButtons = QSharedPointer<QDialogButtonBox>(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel));
    connect(m_qdbbButtons.data(), SIGNAL(accepted()), this, SLOT(accepted()));
    connect(m_qdbbButtons.data(), SIGNAL(rejected()), this, SLOT(rejected()));

    mainLayout->addWidget(m_qdbbButtons.data());
}

void CScreenShot::accepted()
{
    m_width = m_qsbWidthSpin->value();
    m_height = m_qsbHeightSpin->value();
    m_qsFileName = m_qleFileNameEdit->text();
    close();
}

void CScreenShot::rejected()
{
    m_width = m_qsbWidthSpin->value();
    m_height = m_qsbHeightSpin->value();

    m_qsFileName = "";
    close();
}

void CScreenShot::clicked()
{
	QString supportedExtensions;
	QList<QByteArray> formats = QImageWriter::supportedImageFormats();
    for (int i = 0; i < formats.count(); ++i)
	{
//		if (i < (formats.count() - 1))
			supportedExtensions += "Image file (*." + QString(formats.at(i)) + ")\n";
	}

	QString result = QFileDialog::getSaveFileName(this, tr("Browse location"), "screenshots", supportedExtensions); 

    m_qleFileNameEdit->setText(result);
}
