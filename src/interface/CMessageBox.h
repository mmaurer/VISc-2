#ifndef CMESSAGEBOX_H
#define CMESSAGEBOX_H

#include <QtGui>
#include <QToolBar>
#include <QListWidget>
#include <QAction>

#include "../visc.h"

#if defined(WIN32) && defined(_DEBUG)
   #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
   #define new DEBUG_NEW
#endif

class CMessageBox : public QWidget
{
	Q_OBJECT

	public:
		~CMessageBox(void);

		static CMessageBox* getInstance();
		void writeMessage(QString, VISc::EMessageType);
		void method();

	protected:
		CMessageBox(void);

	private slots:
		void informationClicked();
		void warningClicked();
		void errorClicked();
		void clearAllClicked();

	private:
		void updateMessages();

		static bool instanceFlag;
		static CMessageBox *mbInstance;

		QToolBar *controls;
		QListWidget *messages;
		QAction *information;
		QAction *warning;
		QAction *error;
		QAction *clearAll;
};
#endif
