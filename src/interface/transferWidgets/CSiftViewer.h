#ifndef CSIFTVIEWER_H
#define CSIFTVIEWER_H

#include <QtGui>

#include "../CBasicTransferWidget.h"
#include "CTransferFunction.h"

#include "../../algs/sift3d/CKeypoint.h"

#if defined(WIN32) && defined(_DEBUG)
#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif

class QRadioButton;
class QCheckBox;

class CSiftViewer : public CBasicTransferWidget
{
   Q_OBJECT

public:
   CSiftViewer();
   ~CSiftViewer()
   {
      delete tf;
   }

   // Inherited methods
   void render();
   int load(QString fileName);
   int save(QString fileName);
   void zero();
   void reset();
   void clear() { tf->clear(); } ;
   QString getExtension();
   QString getDescription();
   void setContextMenu(QMenu *);
   int getWidth();
   QImage getTransferFunction(int);
   void computeHistogram(int, bool);
   bool isChanged() { return (this->tf->isChanged() || changed); };
   void setUnchanged() { this->tf->setUnchanged(); };
   bool isDirtyVolume() { return dirtyVolume; };
   void setCleanVolume() { dirtyVolume = false; };

   private slots:
      void hSpacingValueChanged(int);
      void vSpacingValueChanged(int);
      void radioButtonClicked(bool);
      void openFileClicked();
      void colorPickerAction();
      void renderModeAction(int);
      void contrastVolumeAction();

private:
   CTransferFunction *tf;
   QString lastDirectory;
   QList<sift3dlib::CKeypoint> keypoints;
   VISc::ERenderMode renderMode;
   QColor color;
   QColor selectedKeypoint;
   bool changed;
   bool dirtyVolume;

   // Interface
   QRadioButton *red, *green, *blue, *opacity;
   QSpinBox *hSpacing, *vSpacing;
   QPushButton *openFile, *contrastVolume;
   QCheckBox *renderModeCheckBox;
   QLabel *colorLabel;
   QLineEdit *colorShower;
   QPushButton *colorPicker;
   QSpinBox *showKeypoint;
   QLabel *numKeypointsLoaded;
   QSpinBox *radius, *contrast;
};

#endif
