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

#ifndef CBASICTRANSFERWIDGET_H
#define CBASICTRANSFERWIDGET_H

#include <QWidget>
#include <QList>

#include "../visc.h"

class CVolume;
class QMenu;

#if defined(WIN32) && defined(_DEBUG)
#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif

class CBasicTransferWidget : public QWidget
{
public:
   CBasicTransferWidget(QString extension, QString description, VISc::ETransferWidgetOperations operations, Qt::DockWidgetAreas dockAreas) :
      m_volumeListPtr(NULL),
      m_contextMenuPtr(NULL),
      m_currentVolumeIndex(0),
      m_extension(extension),
      m_description(description),
      m_availableOperations(operations),
      m_allowableDockAreas(dockAreas)
      { }
   virtual ~CBasicTransferWidget() { }

   void setLists(QList< QSharedPointer<CVolume> > *volumeList) { m_volumeListPtr = volumeList; }
   virtual Qt::DockWidgetAreas getAllowedDockArea() { return m_allowableDockAreas; }
   VISc::ETransferWidgetOperations getAvailableOperations() { return m_availableOperations; }

   // Non-virtual
   QString getExtension() { return m_extension; }
   QString getDescription() { return m_description; }

   // Virtual
   virtual void setContextMenu(QMenu *menu) { m_contextMenuPtr = menu; }
   virtual void render() { }
   virtual void zero() { }
   virtual void reset() { }
   virtual void clear() { }
   virtual int load(QString fileName) { return 0; }
   virtual int save(QString fileName) { return 0; }
   virtual int getWidth() = 0;
   virtual QImage getTransferFunction(int) = 0;
   virtual void computeHistogram(int, bool) = 0;
   virtual bool isChanged() { return false; }
   virtual void setUnchanged() { }
   virtual bool isDirtyVolume() { return false; }
   virtual void setCleanVolume() { }
   virtual void destroy() { }

protected:
   QList< QSharedPointer<CVolume> > *m_volumeListPtr; // Doesn't take ownership
   QMenu *m_contextMenuPtr; // Doesn't take ownership
   int m_currentVolumeIndex;

   QString m_extension;
   QString m_description;
   VISc::ETransferWidgetOperations m_availableOperations;
   Qt::DockWidgetAreas m_allowableDockAreas;
};

#endif
