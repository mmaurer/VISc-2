#ifndef CBASICCLUSTERINGVISUALIZER_H
#define CBASICCLUSTERINGVISUALIZER_H

#include <QtGui>
#include "..\CBasicTransferWidget.h"
#include "..\..\algebra\CVector.h"

#include "../../algs/CThreadWorker.h"

#if defined(WIN32) && defined(_DEBUG)
#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif

class CBasicClusteringVisualizer : public CBasicTransferWidget
{
   Q_OBJECT

public:
   enum EMetric { mDensity, mUp, mDown, mFirstDerivative, mSecondDerivative, mConnectivity };

   CBasicClusteringVisualizer();
   virtual ~CBasicClusteringVisualizer() { }

   // Virtual methods
   virtual int getWidth() { return 310; }
   virtual QImage getTransferFunction(int);
   virtual void computeHistogram(int, bool);
   virtual bool isChanged() { return m_bChanged; }
   virtual void setUnchanged() { m_bChanged = false; }
   virtual bool isDirtyVolume() { return m_bDirty; }
   virtual void setCleanVolume() { m_bDirty = false; }
   virtual void destroy();

public slots:
   void terminateWorker(bool &value) { value = m_bTerminate; }

private slots:
   void itemClicked ( QTreeWidgetItem * item, int column );
   void updateVolumeSecondDerivative();
   void updateVolumeFirstDerivative();
   void updateVolumeDownDensity();
   void updateVolumeUpDensity();
   void updateVolumeDensity();
   void itemDoubleClicked ( QTreeWidgetItem * item, int column );
   void clicked ( bool checked );
   void progressUpdated(int value);
   void clusterComputationFinished();

protected:
   void updateVisibleItems();
   //void computeClusters();
   void checkOne(QTreeWidgetItem *);
   //void computeUpClusters(int nclusters, int value);
   void updateTransferFunction(QTreeWidgetItem *);
   //void computeFirstDerivativeClusters(int nclusters, CVector &gradient);
   //void computeSecondDerivativeClusters(int nclusters, CVector &gradient);
   //void computeDensityClusters(int nclusters, int value);
   //void computeDownClusters(int nclusters, int value);

   // Clustering variables
   QList<intCluster> m_qlCpDensity; // Previous cluster centers
   QList<intCluster> m_qlCpUp;
   QList<intCluster> m_qlCpDown;
   QList<vecCluster> m_qlCpFirstDerivative;
   QList<vecCluster> m_qlCpSecondDerivative;

   //QList<intCluster> m_qlCnDensity; // Next cluster centers
   //QList<intCluster> m_qlCnUp;
   //QList<intCluster> m_qlCnDown;
   //QList<vecCluster> m_qlCnFirstDerivative;
   //QList<vecCluster> m_qlCnSecondDerivative;

   bool m_bTerminate;
   bool m_bDirty;
   bool m_bChanged;

   QImage m_qiTransferFunctionImage;
   CVolume *m_localVolumePtr;

   // Interface variables
   QSharedPointer<QVBoxLayout> m_qvlMainLayout;
   QSharedPointer<QVBoxLayout> m_qvlTWidgetLayout;
   QSharedPointer<QScrollArea> m_saInternalPanel;
   QSharedPointer<QTreeWidget> m_qtwVisibleItems;
   QSharedPointer<QProgressBar> m_qpbComputationProgress;
   QSharedPointer<QSpinBox> m_qsbNumIterations;
   QSharedPointer<QPushButton> m_qpbCalculate;
   QSharedPointer<QPushButton> m_qpbCancel;
   QSharedPointer<QSpinBox> m_qsbInitialNumClusters;
   QSharedPointer<QLabel> m_qlNumIterationsLabel;
   QSharedPointer<QLabel> m_qlNumInitialClustersLabel;

   QThread m_tcc; 
   QThreadWorker m_tw;
};

#endif
