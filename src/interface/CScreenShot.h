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

#ifndef CSCREENSHOT_H
#define CSCREENSHOT_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QDialogButtonBox>

#if defined(WIN32) && defined(_DEBUG)
#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif

class CScreenShot : public QDialog
{
    Q_OBJECT

public:
    CScreenShot(QSize size);
    virtual ~CScreenShot() { }

    int getWidth() const {return m_width; }
    int getHeight() const { return m_height; }
    QString getFileName() const { return m_qsFileName; }

private slots:
    void accepted();
    void clicked();
    void rejected();

private:
    int m_width, m_height;
    QString m_qsFileName;

    QSharedPointer<QLabel> m_qlWidthLabel, m_qlHeightLabel;
    QSharedPointer<QSpinBox> m_qsbWidthSpin, m_qsbHeightSpin;

    QSharedPointer<QLabel> imageFormatlabel;
    QSharedPointer<QComboBox> m_qcbImageFormatCombo;

    QSharedPointer<QLabel> m_qlFileNameLabel;
    QSharedPointer<QLineEdit> m_qleFileNameEdit;
    QSharedPointer<QPushButton> m_qpbFileNameButton;

    QSharedPointer<QDialogButtonBox> m_qdbbButtons;
};

#endif
