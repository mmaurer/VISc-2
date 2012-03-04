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
