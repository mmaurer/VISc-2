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

#include "CMessageBox.h"

#include <QVBoxLayout>

bool CMessageBox::instanceFlag = false;
CMessageBox* CMessageBox::mbInstance = NULL;

CMessageBox::CMessageBox(void)
{
	this->setContentsMargins(0, 0, 0, 0);

	messages = new QListWidget();

	controls = new QToolBar(tr("Messages controls"));

	information = new QAction(QIcon(":/resource/info.png"), tr("Information"), this);
	information->setCheckable(true);
	information->setChecked(true);
	information->setStatusTip(tr("Show informational messages"));
	connect(information, SIGNAL(triggered()), this, SLOT(informationClicked()));
	warning     = new QAction(QIcon(":/resource/warning.png"), tr("Warning"), this);
	warning->setCheckable(true);
	warning->setChecked(true);
	warning->setStatusTip(tr("Show warning messages"));
	connect(warning, SIGNAL(triggered()), this, SLOT(warningClicked()));
	error       = new QAction(QIcon(":/resource/error.png"), tr("Error"), this);
	error->setCheckable(true);
	error->setChecked(true);
	error->setStatusTip(tr("Show error messages"));
	connect(error, SIGNAL(triggered()), this, SLOT(errorClicked()));
	clearAll    = new QAction(QIcon(":/resource/clearAll.png"), tr("Clear all"), this);
	clearAll->setStatusTip(tr("Clear all messages"));
	connect(clearAll, SIGNAL(triggered()), this, SLOT(clearAllClicked()));

	controls->addAction(information);
	controls->addAction(warning);
	controls->addAction(error);
	controls->addSeparator();
	controls->addAction(clearAll);
	//controls->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	controls->setContentsMargins(0, 0, 0, 0);
	
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
//#if QT_VERSION >= QT_VERSION_CHECK(4, 3, 0)
//	mainLayout->setContentsMargins(2, 2, 2, 2);
//#endif
	mainLayout->addWidget(controls);
	mainLayout->addWidget(messages);
}

CMessageBox::~CMessageBox(void)
{
	instanceFlag = false;
}

CMessageBox* CMessageBox::getInstance()
{
	if (!instanceFlag)
	{
		mbInstance = new CMessageBox();
		instanceFlag = true;
	}

	return mbInstance;
}

void CMessageBox::writeMessage(QString text, VISc::EMessageType mtype)
{
     QListWidgetItem *newItem = new QListWidgetItem(messages, mtype);
     newItem->setText(text);
	 switch (mtype)
	 {
		 case VISc::mtError:
						newItem->setIcon(QIcon(":/resource/error.png"));
						break;
		 case VISc::mtInformation:
						newItem->setIcon(QIcon(":/resource/info.png"));
						break;
		 case VISc::mtWarning:
						newItem->setIcon(QIcon(":/resource/warning.png"));
						break;
	 }
     messages->insertItem(0, newItem);
}

void CMessageBox::informationClicked()
{
	updateMessages();
}

void CMessageBox::warningClicked()
{
	updateMessages();
}

void CMessageBox::errorClicked()
{
	updateMessages();
}

void CMessageBox::updateMessages()
{
	bool i, e, w;
	i = information->isChecked();
	e = error->isChecked();
	w = warning->isChecked();

	for ( int j = 0; j < messages->count(); j++)
	{
		int data = messages->item(j)->type();

		if (data == VISc::mtError)
			messages->item(j)->setHidden(!e);
		else if (data == VISc::mtInformation)
			messages->item(j)->setHidden(!i);
		else if (data == VISc::mtWarning)
			messages->item(j)->setHidden(!w);
	}
}

void CMessageBox::clearAllClicked()
{
	messages->clear();
}
