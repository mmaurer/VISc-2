#include "CSiftViewer.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QPushButton>
#include <QCheckBox>
#include <QFileDialog>
#include <QMenu>

#include "../../algs/sift3d/CXMLKeypointReader.h"

CSiftViewer::CSiftViewer(void) :
CBasicTransferWidget( tr("1tf"), tr("1D Transfer Function with Keypoint Viewer (3D Sift)"), VISc::twoLoad | VISc::twoSave | VISc::twoZero | VISc::twoReset, Qt::RightDockWidgetArea)
{
	renderMode = VISc::rmWireframe;
	color = QColor(255, 255, 0);
	selectedKeypoint = QColor(255, 0, 0);
	changed = false;

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

	// Open File
	openFile = new QPushButton(tr("Load Keypoints"));
	connect(openFile, SIGNAL(clicked()), this, SLOT(openFileClicked()));

	// Color Picker
	QHBoxLayout *cLayout = new QHBoxLayout();
	colorLabel = new QLabel(tr("Keypoints Color:"));
	colorShower = new QLineEdit();
	colorShower->setEnabled(false);
	QPalette palette( colorShower->palette() );
	palette.setColor( QPalette::Base, color );
	colorShower->setPalette(palette);

	colorPicker = new QPushButton(tr("Color"));
	connect(colorPicker, SIGNAL(clicked()), this, SLOT(colorPickerAction()));
	cLayout->addWidget(colorLabel);
	cLayout->addWidget(colorShower);
	cLayout->addWidget(colorPicker);

	// Render mode
	renderModeCheckBox = new QCheckBox(tr("Wireframe"));
	renderModeCheckBox->setChecked(true);
	connect(renderModeCheckBox, SIGNAL(stateChanged(int)), this, SLOT(renderModeAction(int)));

	// Show Keypoint Group
	QGroupBox *showKeypointGroup = new QGroupBox(tr("Show Selected Keypoint"));
	QVBoxLayout *skvl = new QVBoxLayout();
	
	numKeypointsLoaded = new QLabel(tr("No keypoints loaded"));
	skvl->addWidget(numKeypointsLoaded);

	QHBoxLayout *skl = new QHBoxLayout();

	QLabel *showKeypointLabel = new QLabel(tr("Show keypoint:"));
	showKeypoint = new QSpinBox();
	showKeypoint->setMaximum( -1 );
	showKeypoint->setMinimum(-1);
	showKeypoint->setValue( -1 );
	skl->addWidget(showKeypointLabel);
	skl->addWidget(showKeypoint);
	skvl->addLayout( skl );
	showKeypointGroup->setLayout( skvl );

	// Change contrast of the volume
	QGridLayout *neighbourhoodLayout = new QGridLayout();
	radius = new QSpinBox();
	radius->setMinimum(1);
	radius->setValue(4);
	contrast = new QSpinBox();
	contrast->setMaximum(255);
	contrast->setValue(100);
	neighbourhoodLayout->addWidget(new QLabel("Radius"), 0, 0, 1, 1);
	neighbourhoodLayout->addWidget(radius, 0, 1, 1, 1);
	neighbourhoodLayout->addWidget(new QLabel("Max contrast"), 1, 0, 1, 1);
	neighbourhoodLayout->addWidget(contrast, 1, 1, 1, 1);
	contrastVolume = new QPushButton(tr("Enhance keypoints neighbourhood"));
	connect(contrastVolume, SIGNAL(clicked()), this, SLOT(contrastVolumeAction()));


	// Final assembly of the right panel
	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->setMargin(0);
	mainLayout->addWidget(tf);
	mainLayout->addLayout(radioLayout);
	mainLayout->addLayout(spacingLayout);
	mainLayout->addWidget(openFile);
	mainLayout->addLayout(cLayout);
	mainLayout->addWidget(renderModeCheckBox);
	mainLayout->addWidget( showKeypointGroup );
	mainLayout->addLayout( neighbourhoodLayout );
	mainLayout->addWidget( contrastVolume );
	mainLayout->addStretch(1);

	this->setLayout(mainLayout);

	
	// Test of keypoints
	//int j = 0;
	//for (int i = 0; i < 5; i++)
	//{
	//	sift3dlib::CKeypoint k(j, j, j, 1.0, j / 10.0 + 1.0, 0.78, 0.18);
	//	keypoints.append(k);

	//	j += 10;
	//}
	//sift3dlib::CKeypoint k(0, 0, 0, 1.0, 1.0, 0.7854, 0.9553);
	//keypoints.append(k);
	//sift3dlib::CKeypoint k2(16, 16, 16, 1.0, 1.0, 0.78, 0.18);
	//keypoints.append(k2);
	//sift3dlib::CKeypoint k3(32, 32, 32, 1.0, 1.0, 0.78, 0.18);
	//keypoints.append(k3);

	dirtyVolume = false;
}

// Inherited methods
void CSiftViewer::render()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	for (int v = 0; v < m_volumeListPtr->count(); v++)
	{
		//// Transforms the volume to a texturable object
		//// 3D Textures must have power of 2 dimensions
		//float log2 = log10f(2.0f);
		//float w = log10f(tmpVolumeList->at(v)->getWidth()) / log2;
		//float h = log10f(tmpVolumeList->at(v)->getHeight()) / log2;
		//float d = log10f(tmpVolumeList->at(v)->getDepth()) / log2;

		//w = powf(2.0f, ceilf(w));
		//h = powf(2.0f, ceilf(h));
		//d = powf(2.0f, ceilf(d));

		//if ((w != tmpVolumeList->at(v)->getWidth()) ||
		//	(h != tmpVolumeList->at(v)->getHeight()) ||
		//	(d != tmpVolumeList->at(v)->getDepth()))
		//{
		//	// Do something
		//}


		for (int i = 0; i < keypoints.count(); i++)
		{
			sift3dlib::CKeypoint &k = keypoints[i];

			if (i == showKeypoint->value())
				glColor3f(selectedKeypoint.redF(), selectedKeypoint.greenF(), selectedKeypoint.blueF());
			else
				glColor3f(color.redF(), color.greenF(), color.blueF());
			glPushMatrix();
				glTranslatef(-(m_volumeListPtr->at(v)->getWidth()  / 2.0 - 1.25/*0.8325*2*/) + k.getX() /*/ 2.0*/, 
							 -(m_volumeListPtr->at(v)->getHeight() / 2.0 - 1.25) + k.getY() /*/ 2.0*/, 
							 -(m_volumeListPtr->at(v)->getDepth()  / 2.0 - 1.25) + k.getZ() /*/ 2.0*/);
				glTranslatef(-1.25, -1.25, -1.25);
				glScalef(sqrt(k.getScale()), 
					     sqrt(k.getScale()), 
						 sqrt(k.getScale()));

				// Rotation of keypoint
				glRotated(k.getOrientation1() / 3.14159265 * 180.0, 0.0, 0.0, 1.0);
				glRotated(k.getOrientation2() / 3.14159265 * 180.0, 0.0, 1.0, 0.0);
				

				// Drawing two 'concentric' triangles
				if (renderMode == VISc::rmWireframe)
				{
					glBegin(GL_LINES);
						// First triangle
						glVertex3f(static_cast<GLfloat>(0.0), static_cast<GLfloat>(0.0), static_cast<GLfloat>(1.667*2));
						glVertex3f(static_cast<GLfloat>(2.5), static_cast<GLfloat>(0.0), static_cast<GLfloat>(-1.667));
						
						glVertex3f(static_cast<GLfloat>(2.5), static_cast<GLfloat>(0.0), static_cast<GLfloat>(-1.667));
						glVertex3f(static_cast<GLfloat>(-2.5), static_cast<GLfloat>(0.0), static_cast<GLfloat>(-1.667));

						glVertex3f(static_cast<GLfloat>(-2.5), static_cast<GLfloat>(0.0), static_cast<GLfloat>(-1.667));
						glVertex3f(static_cast<GLfloat>(0.0), static_cast<GLfloat>(0.0), static_cast<GLfloat>(1.667*2));

						// Second Triangle
						glVertex3f(static_cast<GLfloat>(0.0), static_cast<GLfloat>(0.0), static_cast<GLfloat>(1.667*2));
						glVertex3f(static_cast<GLfloat>(0.0), static_cast<GLfloat>(2.5), static_cast<GLfloat>(-1.667));
						
						glVertex3f(static_cast<GLfloat>(0.0), static_cast<GLfloat>(2.5), static_cast<GLfloat>(-1.667));
						glVertex3f(static_cast<GLfloat>(0.0), static_cast<GLfloat>(-2.5), static_cast<GLfloat>(-1.667));

						glVertex3f(static_cast<GLfloat>(0.0), static_cast<GLfloat>(-2.5), static_cast<GLfloat>(-1.667));
						glVertex3f(static_cast<GLfloat>(0.0), static_cast<GLfloat>(0.0), static_cast<GLfloat>(1.667*2));
					glEnd();
				}
				else if (renderMode == VISc::rmRealTime || renderMode == VISc::rmUnrealTime)
				{
					glBegin(GL_TRIANGLES);
						// First triangle
						glVertex3f(static_cast<GLfloat>(0.0), static_cast<GLfloat>(0.0), static_cast<GLfloat>(1.667*2));
						glVertex3f(static_cast<GLfloat>(2.5), static_cast<GLfloat>(0.0), static_cast<GLfloat>(-1.667));
						glVertex3f(static_cast<GLfloat>(-2.5), static_cast<GLfloat>(0.0), static_cast<GLfloat>(-1.667));

						// Second Triangle
						glVertex3f(static_cast<GLfloat>(0.0), static_cast<GLfloat>(0.0), static_cast<GLfloat>(1.667*2));
						glVertex3f(static_cast<GLfloat>(0.0), static_cast<GLfloat>(2.5), static_cast<GLfloat>(-1.667));
						glVertex3f(static_cast<GLfloat>(0.0), static_cast<GLfloat>(-2.5), static_cast<GLfloat>(-1.667));
					glEnd();
				}
			glPopMatrix();
		}
	}

	glPopAttrib();
}
int CSiftViewer::load(QString fileName)
{
	tf->setChanged();
	return tf->load(fileName);
}

int CSiftViewer::save(QString fileName)
{
	return tf->save(fileName);	
}

void CSiftViewer::zero()
{
	tf->setChanged();
	tf->zero();
}

void CSiftViewer::reset()
{
	tf->setChanged();
	tf->reset();
}
QString CSiftViewer::getExtension()
{
	return "1tf";
}
QString CSiftViewer::getDescription()
{
	return tr("1D Transfer Function");
}
void CSiftViewer::setContextMenu(QMenu *menu)
{ 
	this->m_contextMenuPtr = menu;
	tf->setContextMenu(menu);
}
int CSiftViewer::getWidth()
{
	return tf->getWidth();
}
QImage CSiftViewer::getTransferFunction(int)
{
	return tf->getTransferFunction();
}
void CSiftViewer::computeHistogram(int volumeIndex, bool resetTransferFunction)
{
	if ((m_volumeListPtr->count() > 0) && (volumeIndex < m_volumeListPtr->count()))
		tf->computeHistogram(*(m_volumeListPtr->at(volumeIndex)), resetTransferFunction);
}
//end of inherited members

// Slots
void CSiftViewer::hSpacingValueChanged(int value)
{
	tf->setGridX(value);
}
void CSiftViewer::vSpacingValueChanged(int value)
{
	tf->setGridY(value);
}
void CSiftViewer::radioButtonClicked(bool)
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
void CSiftViewer::openFileClicked()
{
	QString fileName;

	fileName = QFileDialog::getOpenFileName(this, tr("Load Keypoints"), lastDirectory, tr("3D Sift Keypoints (*.xml)"));
	
	if (fileName.isEmpty())
		return;

	lastDirectory = fileName;
	
	CXMLKeypointReader reader;
	reader.loadKeypoints(fileName);

	std::vector<CKeypoint> tmp = reader.getKeypoints();
	this->keypoints.clear();
	for (unsigned int i = 0; i < tmp.size(); i++)
		this->keypoints.push_back( tmp.at(i) );

	showKeypoint->setMaximum( this->keypoints.size() );
	numKeypointsLoaded->setText(tr("%1 keypoint(s) loaded").arg(this->keypoints.size()));
}
void CSiftViewer::colorPickerAction()
{
	color = QColorDialog::getColor(this->color, this);
	//tmpView->setBackgroundColor( tmpColor );
	QPalette palette( colorShower->palette() );
	palette.setColor( QPalette::Base, color );
	colorShower->setPalette(palette);
	this->changed = true;
}
void CSiftViewer::renderModeAction(int state)
{
	if (state == Qt::Checked)
		renderMode = VISc::rmWireframe;
	else if (state == Qt::Unchecked)
		renderMode = VISc::rmRealTime;
	this->changed = true;
}
void CSiftViewer::contrastVolumeAction()
{

	CVolume tmpVolume(m_volumeListPtr->at(0)->getWidth(), 
					  m_volumeListPtr->at(0)->getHeight(), 
					  m_volumeListPtr->at(0)->getDepth(), 1);
	CVolume *tmp = m_volumeListPtr->at(0).data();
	
	for (int i = 0; i < tmp->totalVoxels(); i++)
	{
		double value = tmp->getVoxel(i) / 255.0 * (float) contrast->value();

		tmpVolume.setVoxel(i, int(value+0.5)); 
	}

	for (int i = 0; i < keypoints.size(); i++)
	{
		int r = radius->value();
		for (int x = (keypoints[i].getX() - r); x <= (keypoints[i].getX() + r); x++)
			for (int y = (keypoints[i].getY() - r); y <= (keypoints[i].getY() + r); y++)
				for (int z = (keypoints[i].getZ() - r); z <= (keypoints[i].getZ() + r); z++)
				{
					int dx = x - keypoints[i].getX();
					int dy = y - keypoints[i].getY();
					int dz = z - keypoints[i].getZ();
					
					float dist = sqrt((float) (dx*dx + dy*dy + dz*dz));
					if (dist < r)
						tmpVolume.setVoxel(x, y, z, tmp->getVoxel(x, y, z));
				}
	}

	*(m_volumeListPtr->at(0)) = tmpVolume;
	dirtyVolume = true;
}
// end of slots
