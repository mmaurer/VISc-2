#ifndef CBASICTRANSFERWIDGET_H
#define CBASICTRANSFERWIDGET_H

#include "../visc.h"
#include <QtGui>

class CVolume;

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
