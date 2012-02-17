#include "CMultiresolution.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QTimer>

CMultiresolution::CMultiresolution(void) :
CBasicTransferWidget(tr("1tf"), tr("1D Transfer Function"), VISc::twoLoad | VISc::twoSave | VISc::twoZero | VISc::twoReset, Qt::RightDockWidgetArea)
{
	renderMode = rmNormal;

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

	// Group Box
	information = new QGroupBox(tr("Clustering Binary Tree"), this);
	QGridLayout *infolayout = new QGridLayout();

	QLabel *ltmp = new QLabel(tr("Current CBT"));
	ltmp->setAlignment(Qt::AlignCenter);
	infolayout->addWidget(ltmp, 0, 1);
	ltmp = new QLabel(tr("Multiresolution<br>Parameters"));
	ltmp->setAlignment(Qt::AlignCenter);
	infolayout->addWidget(ltmp, 0, 2);
	lthreshold = new QLabel(tr("0"));
	lthreshold->setAlignment(Qt::AlignCenter);
	infolayout->addWidget(new QLabel(tr("Threshold")), 1, 0);
	infolayout->addWidget(lthreshold, 1, 1);
	infolayout->addWidget(new QLabel(tr("Depth")), 2, 0);
	ldepth = new QLabel(tr("0"));
	ldepth->setAlignment(Qt::AlignCenter);
	infolayout->addWidget(ldepth, 2, 1);
	depth = new QSpinBox();
	depth->setValue(10);
	infolayout->addWidget(depth, 2, 2);
	infolayout->addWidget(new QLabel(tr("Maximum allowed<br>error")), 3, 0);
	lminError = new QLabel(tr("0"));
	lminError->setAlignment(Qt::AlignCenter);
	infolayout->addWidget(lminError, 3, 1);
	minError = new QDoubleSpinBox();
	minError->setMaximum(255);
	minError->setValue(20);
	infolayout->addWidget(minError, 3, 2);
	infolayout->addWidget(new QLabel(tr("Minimum Cluster<br>Size")), 4, 0);
	minGroupSize = new QSpinBox();
	minGroupSize->setMinimum(2);
	minGroupSize->setMaximum(1000);
	infolayout->addWidget(minGroupSize, 4, 2);
	numNodes = new QLabel("0");
	numNodes->setAlignment(Qt::AlignCenter);
	infolayout->addWidget(new QLabel(tr("Number of nodes")), 5, 0);
	infolayout->addWidget(numNodes, 5, 1);


	compute = new QPushButton(tr("Compute CBT"));
	infolayout->addWidget(compute, 6, 1, 1, 2);
	connect(compute, SIGNAL(clicked()), this, SLOT(computeClicked()));
	regen = new QPushButton(tr("Regenerate Volume"));
	regen->setEnabled(false);
	infolayout->addWidget(regen, 7, 1, 1, 2);
	connect(regen, SIGNAL(clicked()), this, SLOT(regenVolume()));
	generationType = new QComboBox();
	generationType->setEnabled(true);
	generationType->addItem(tr("Normal"));
	generationType->addItem(tr("M Clusters"));
	infolayout->addWidget(generationType, 6, 0, 1, 1);


	processingType = new QComboBox();
	processingType->setEnabled(false);
	processingType->addItem(tr("Level-based"));
	processingType->addItem(tr("Error-based"));
	infolayout->addWidget(processingType, 7, 0, 1, 1);

	switchVolumes = new QPushButton(tr("Switch Volumes"));
	infolayout->addWidget(switchVolumes, 8, 1, 1, 2);
	connect(switchVolumes, SIGNAL(clicked()), this, SLOT(switchVolumesAction()));

	information->setLayout(infolayout);

	// Final assembly of the right panel
	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->setMargin(0);
	mainLayout->addWidget(tf);
	mainLayout->addLayout(radioLayout);
	mainLayout->addLayout(spacingLayout);
	mainLayout->addWidget(information);
	mainLayout->addStretch(1);

	this->setLayout(mainLayout);

	// CBT
	cbt = NULL;

	this->dirtyVolume = false;

	// Timer
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(200);
}

CMultiresolution::~CMultiresolution(void)
{
	if (m_volumeListPtr->size() > 0)
		*(m_volumeListPtr->at(0)) = privateVolume;

	delete tf;
}

// Inherited methods
void CMultiresolution::render()
{ 
	if (m_volumeListPtr->size() < 1)
		return;

	if (renderMode == rmNormal)
		return;

	glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushMatrix();
			glTranslatef(-m_volumeListPtr->at(0)->getWidth() / 2.0,
				        -m_volumeListPtr->at(0)->getHeight() / 2.0, 
					    -m_volumeListPtr->at(0)->getDepth() / 2.0);
			cbt->renderMarchingClusters(m_volumeListPtr->at(0).data(), ttLevelBased, depth->value(), initialLevel);
		glPopMatrix();
	glPopAttrib();
}
int CMultiresolution::load(QString fileName)
{
	tf->setChanged();
	return tf->load(fileName);
}

int CMultiresolution::save(QString fileName)
{
	return tf->save(fileName);	
}

void CMultiresolution::zero()
{
	tf->setChanged();
	tf->zero();
}

void CMultiresolution::reset()
{
	tf->setChanged();
	tf->reset();
}
QString CMultiresolution::getExtension()
{
	return "1tf";
}
QString CMultiresolution::getDescription()
{
	return tr("1D Transfer Function");
}
void CMultiresolution::setContextMenu(QMenu *menu)
{ 
	this->m_contextMenuPtr = menu;
	tf->setContextMenu(menu);
}
int CMultiresolution::getWidth()
{
	return tf->getWidth()+2;
}
QImage CMultiresolution::getTransferFunction(int)
{
	return tf->getTransferFunction();
}
void CMultiresolution::computeHistogram(int volumeIndex, bool resetTransferFunction)
{
	if ((m_volumeListPtr->count() > 0) && (volumeIndex < m_volumeListPtr->count()))
	{
		if ((m_volumeListPtr->at(volumeIndex)->getWidth() > UCHAR_MAX) ||
			(m_volumeListPtr->at(volumeIndex)->getHeight() > UCHAR_MAX) ||
			(m_volumeListPtr->at(volumeIndex)->getDepth() > UCHAR_MAX))
		{
			CMessageBox::getInstance()->writeMessage(tr("Volume dimensions is bigger than internal representation can handle"), VISc::mtError);
			this->setEnabled( false );
		}
		privateVolume = *(m_volumeListPtr->at(volumeIndex));
		tf->computeHistogram(*(m_volumeListPtr->at(volumeIndex)), resetTransferFunction);
	}
}
//end of inherited members

// Slots
void CMultiresolution::hSpacingValueChanged(int value)
{
	tf->setGridX(value);
}
void CMultiresolution::vSpacingValueChanged(int value)
{
	tf->setGridY(value);
}
void CMultiresolution::radioButtonClicked(bool)
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
void CMultiresolution::computeClicked()
{
	if (generationType->currentText() == tr("Normal"))
		renderMode = rmNormal;
	else if (generationType->currentText() == tr("M Clusters"))
		renderMode = rmMarchingClusters;

	QProgressDialog *pDialog = new QProgressDialog(tr("Computing CBT"), tr("Cancel"), 0, 100);
	connect(pDialog, SIGNAL(canceled()), this, SLOT(cancel()));
	pDialog->show();
	pDialog->setValue(0);

	if (cbt != NULL)
		delete cbt;

	CCluster *first = new CCluster(&privateVolume, (char) (int) lthreshold->text().toInt(), tf->getTransferFunction());
	
	if (renderMode == rmNormal)
		first->computeCenter();
	else if (renderMode == rmMarchingClusters)
		first->computeCenterMarchingClusters();

	cbt = new CNode<CCluster>( first );
	pDialog->setValue(10);
	
	if (renderMode == rmNormal)
		cbt->generateCBT(initialLevel, minGroupSize->value(), pDialog);
	else if (renderMode == rmMarchingClusters)
		cbt->generateCBTMarchingClusters(initialLevel, minGroupSize->value(), pDialog);
	
	pDialog->setValue(50);

	int maxDepth = cbt->getDepth(0);
	ldepth->setText(tr("%1").arg(maxDepth));
	int maxError = cbt->getMaxError();
	lminError->setText(tr("%1").arg(maxError));
	int count = cbt->getNumNodes();
	numNodes->setText(tr("%1").arg(count));

	//pDialog->setModal(true);
	pDialog->close();
	regen->setEnabled(true);
	processingType->setEnabled(true);
}
void CMultiresolution::update()
{
	QImage transferfunction = tf->getTransferFunction();
	
	int y = 0;
	while (transferfunction.pixel(y++, 0) <= qRgba(255,255,255,0) && (y < 256));

	lthreshold->setText(tr("%1").arg(y-1));
}
void CMultiresolution::cancel()
{

}
void CMultiresolution::regenVolume()
{
	m_volumeListPtr->at(0)->zeroAll();
	
	if (processingType->currentText() == tr("Level-based"))
		cbt->render(m_volumeListPtr->at(0).data(), ttLevelBased, depth->value(), initialLevel);
	else if (processingType->currentText() == tr("Error-based"))
		cbt->render(m_volumeListPtr->at(0).data(), ttErrorBased, (int) minError->value(), initialLevel);

	this->dirtyVolume = true;
}
void CMultiresolution::switchVolumesAction()
{
	CVolume tmpVolume = *(m_volumeListPtr->at(0));
	*(m_volumeListPtr->at(0)) = privateVolume;
	privateVolume = tmpVolume;

	this->dirtyVolume = true;
}
// end of slots
