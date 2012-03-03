#include "CAbout.h"

CAbout::CAbout(const QPixmap & pixmap, Qt::WindowFlags f) : QSplashScreen(pixmap, f)
{            
   setMinimumWidth(303);
   setMaximumWidth(303);

   setMinimumHeight(599);
   setMaximumHeight(599);

   setWindowFlags(Qt::SplashScreen | Qt::WindowStaysOnTopHint);
   setWindowModality(Qt::WindowModal);
}

void CAbout::keyPressEvent( QKeyEvent * )
{
   close();
}

void CAbout::mousePressEvent( QMouseEvent * )
{
   close();
}
