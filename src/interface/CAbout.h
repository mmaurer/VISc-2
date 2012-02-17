#ifndef CABOUT_H
#define CABOUT_H

#include <QSplashScreen>

#if defined(WIN32) && defined(_DEBUG)
#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif

class CAbout : public QSplashScreen
{
    Q_OBJECT

public:
    CAbout(const QPixmap & pixmap = QPixmap(), Qt::WindowFlags f = 0);
    virtual ~CAbout(void) { }

protected:
    virtual void keyPressEvent ( QKeyEvent * event );
    virtual void mousePressEvent ( QMouseEvent * event );
};

#endif
