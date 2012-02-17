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

CScreenShot::CScreenShot(QSize lastSize)
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QVBoxLayout *sizeLayout = new QVBoxLayout();
	QGroupBox *sizeGroup = new QGroupBox(tr("Screenshot size"));
	widthLabel = new QLabel(tr("Width"));
	widthSpin = new QSpinBox();
	widthSpin->setMinimum(1);
	widthSpin->setMaximum(5000);
	widthSpin->setValue(lastSize.width());
	
	QHBoxLayout *w = new QHBoxLayout();
	w->addWidget(widthLabel);
	w->addWidget(widthSpin);

	heightLabel = new QLabel(tr("Height"));
	heightSpin = new QSpinBox();
	heightSpin->setMinimum(1);
	heightSpin->setMaximum(5000);
	heightSpin->setValue(lastSize.height());

	QHBoxLayout *h = new QHBoxLayout();
	h->addWidget(heightLabel);
	h->addWidget(heightSpin);

	sizeLayout->addLayout(w);
	sizeLayout->addLayout(h);
	//sizeLayout->addWidget(heightLabel);
	//sizeLayout->addWidget(height);
	sizeGroup->setLayout(sizeLayout);
		
	mainLayout->addWidget(sizeGroup);

	//QHBoxLayout *fileFormatLayout = new QHBoxLayout();
	//imageFormatlabel = new QLabel(tr("Image format:"));
	//imageFormatCombo = new QComboBox();
	//QList<QByteArray> formats = QImageWriter::supportedImageFormats();
	//for (int i = 0; i < formats.count(); i++)
	//{
	//	imageFormatCombo->addItem(QString(formats.at(i)));
	//}
	//fileFormatLayout->addWidget(imageFormatlabel);
	//fileFormatLayout->addWidget(imageFormatCombo);
	//mainLayout->addLayout(fileFormatLayout);

	QHBoxLayout *fileNameLayout = new QHBoxLayout();
	fileNameLabel = new QLabel(tr("File name:"));
	fileNameEdit = new QLineEdit();
	fileNameButton = new QPushButton(tr("Browse"));
	connect(fileNameButton, SIGNAL(clicked()), this, SLOT(clicked()));
	fileNameLayout->addWidget(fileNameLabel);
	fileNameLayout->addWidget(fileNameEdit);
	fileNameLayout->addWidget(fileNameButton);
	mainLayout->addLayout(fileNameLayout);

	buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttons, SIGNAL(accepted()), this, SLOT(accepted()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(rejected()));

	mainLayout->addWidget(buttons);
}

CScreenShot::~CScreenShot(void)
{
}

void CScreenShot::accepted()
{
	width = widthSpin->value();
	height = heightSpin->value();
	fileName = fileNameEdit->text();
	this->close();
}

void CScreenShot::rejected()
{
	width = widthSpin->value();
	height = heightSpin->value();

	fileName = "";
	this->close();
}


void CScreenShot::clicked()
{
	QString supportedExtensions;
	QList<QByteArray> formats = QImageWriter::supportedImageFormats();
	for (int i = 0; i < formats.count(); i++)
	{
		if (i < (formats.count() - 1))
			supportedExtensions += "Image file (*." + QString(formats.at(i)) + ")\n";
	}

	QString result = QFileDialog::getSaveFileName(this, tr("Browse location"), "screenshots", supportedExtensions); 

	fileNameEdit->setText(result);
}
