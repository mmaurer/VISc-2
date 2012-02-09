#include "CAbout.h"

CAbout::CAbout(const QPixmap & pixmap, Qt::WindowFlags f) : QSplashScreen(pixmap, f)
{
	this->setMinimumWidth(303);
	this->setMaximumWidth(303);

	this->setMinimumHeight(599);
	this->setMaximumHeight(599);

	this->setWindowFlags(Qt::SplashScreen | Qt::WindowStaysOnTopHint);
	this->setWindowModality(Qt::WindowModal);
}

void CAbout::keyPressEvent ( QKeyEvent * )
{
	this->close();
}

void CAbout::mousePressEvent ( QMouseEvent * )
{
	this->close();
}
