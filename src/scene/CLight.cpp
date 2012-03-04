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

#include "CLight.h"

#include <QInputDialog>

void CLight::properties()
{
    QString currentName;
    if (!m_qsName.isEmpty())
        currentName = m_qsName;

    bool ok(false);
    QString text = QInputDialog::getText(0, QObject::tr("Light name"), QObject::tr("Lightname:"), QLineEdit::Normal, currentName, &ok);

    if (ok && !text.isEmpty())
        m_qsName = text;
}
