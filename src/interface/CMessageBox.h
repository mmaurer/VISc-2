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

#ifndef CMESSAGEBOX_H
#define CMESSAGEBOX_H

#include "../visc.h"

#include <QtGui>
#include <QToolBar>
#include <QListWidget>
#include <QAction>

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
