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

#ifndef CVOLUMEREADERBASE_H
#define CVOLUMEREADERBASE_H

class CVolumeReaderBase
{
public:
    CVolumeReaderBase(CVolume_ &volume) :
        m_volume(volume)
    { }

    int load(QString fileName) = 0;
    int save(QString fileName) = 0;
    QString getLastError(int errno) = 0;

private:
    CVolumeReaderBase() { }
    CVolumeReaderBase(const CVolumeReaderBase &) { }
    CVolumeReaderBase &operator=(const CVolumeReaderBase &volume) { }

    CVolume_ &m_volume;
};

#endif
