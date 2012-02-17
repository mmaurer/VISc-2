#ifndef CMULTIRESOLUTION_H
#define CMULTIRESOLUTION_H

#include <limits.h> 

#include <QRadioButton>

#include "../CBasicTransferWidget.h"
#include "CTransferFunction.h"
#include "../CMessageBox.h"
#include "Multiresolution/CNode.h"
#include "Multiresolution/CCluster.h"
using namespace nmMultiresolution;

#if defined(WIN32) && defined(_DEBUG)
   #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
   #define new DEBUG_NEW
#endif

class QComboBox;
class QPushButton;
class QLabel;
class QSpinBox;
class QDoubleSpinBox;
class QTimer;

class CMultiresolution : public CBasicTransferWidget
{
	Q_OBJECT

	public:
		CMultiresolution(void);
		~CMultiresolution(void);

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
		bool isChanged() { return this->tf->isChanged(); };
		void setUnchanged() { this->tf->setUnchanged(); };
		bool isDirtyVolume() { return this->dirtyVolume; };
		void setCleanVolume() { this->dirtyVolume = false; };

	private slots:
		void hSpacingValueChanged(int);
		void vSpacingValueChanged(int);
		void radioButtonClicked(bool);
		void computeClicked();
		void regenVolume();
		void update();
		void cancel();
		void switchVolumesAction();

	private:
		CTransferFunction *tf;
		CVolume privateVolume;
		bool dirtyVolume;
		static const int initialLevel = 0;
		nmMultiresolution::ERenderMode renderMode;

		// Interface
		QRadioButton *red, *green, *blue, *opacity;
		QSpinBox *hSpacing, *vSpacing;
		QGroupBox *information;

		QSpinBox *depth, *minGroupSize;
		QDoubleSpinBox *minError;
		QLabel *ldepth, *lminError, *lthreshold, *numNodes;
		QPushButton *compute, *regen, *switchVolumes;
		QComboBox *processingType;
		QComboBox *generationType;

		// Timer
		QTimer *timer;

		// CBT
		CNode<CCluster > *cbt;
};

#endif
