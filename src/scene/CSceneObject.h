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

#ifndef CSCENEOBJECT_H
#define CSCENEOBJECT_H

#include <QDateTime>

#if defined(WIN32) && defined(_DEBUG)
   #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
   #define new DEBUG_NEW
#endif

class CSceneObject
{
public:
   CSceneObject() :
   m_bEnabled(false),
   m_qsName("Unnamed"),
   m_qdtTimeStamp(QDateTime::currentDateTime())
   { }
   virtual ~CSceneObject() { }
   
   virtual void render() { }
   virtual void properties() { }
   virtual void getBoundingBox(float &mw, float &pw, float &mh, float &ph, float &md, float &pd) { mw = pw = mh = ph = md = pd = 0.0f; }

   bool getIsEnabled() const { return m_bEnabled; }
   void setIsEnabled(bool value) { m_bEnabled = value; }
   QString getName() const { return m_qsName; }
   void setName(QString name) { m_qsName = name; }
   QDateTime getTimeStamp() const { return m_qdtTimeStamp; }

protected:
   bool m_bEnabled;
   QString m_qsName;
   QDateTime m_qdtTimeStamp;
};

#endif
