#ifndef CTHREADWORKER_H
#define CTHREADWORKER_H

#include <QObject>
#include <QtGui>
#include <QList>

#include "../algebra/CVector.h"

class CVolume;

//namespace
//{
   struct intCluster
   {
      int center;
      int totalNumberElements;
      QColor color;
      QString name;
   };

   struct vecCluster
   {
      CVector center;
      int totalNumberElements;
      QColor color;
      QString name;
   };
//}

class QThreadWorker : public QObject
{
   Q_OBJECT

public:
   QThreadWorker(QList<intCluster> &c1, QList<intCluster> &c2, QList<intCluster> &c3, QList<vecCluster> &c4, QList<vecCluster> &c5) : 
   QObject(),
   m_bTerminate(false),
   m_numClusters(0),
   m_numIterations(0),
   m_localVolumePtr(NULL),
   m_qlCpDensity(c1),
   m_qlCpUp(c2),
   m_qlCpDown(c3),
   m_qlCpFirstDerivative(c4),
   m_qlCpSecondDerivative(c5)
   { }
   ~QThreadWorker() { }

   void setVolume(CVolume *volume) { m_localVolumePtr = volume; }
   void setNumClusters(int value) { m_numClusters = value; }
   void setNumIterations(int value) { m_numIterations = value; }
   //void terminate() { m_terminate = true; }

public slots:
   void cluster();

signals:
   void terminate(bool &value);
   void progressUpdate(int value);

private:
   void computeClusters();
   void computeFirstDerivativeClusters(int nclusters, CVector &gradient);
   void computeSecondDerivativeClusters(int nclusters, CVector &gradient);
   void computeDensityClusters(int nclusters, int value);
   void computeUpClusters(int nclusters, int value);
   void computeDownClusters(int nclusters, int value);

   bool m_bTerminate;
   int m_numClusters;
   int m_numIterations;
   CVolume *m_localVolumePtr;

   QList<intCluster> &m_qlCpDensity; // Previous cluster centers
   QList<intCluster> &m_qlCpUp;
   QList<intCluster> &m_qlCpDown;
   QList<vecCluster> &m_qlCpFirstDerivative;
   QList<vecCluster> &m_qlCpSecondDerivative;

   QList<intCluster> m_qlCnDensity; // Next (temporary) cluster centers
   QList<intCluster> m_qlCnUp;
   QList<intCluster> m_qlCnDown;
   QList<vecCluster> m_qlCnFirstDerivative;
   QList<vecCluster> m_qlCnSecondDerivative;
};

#endif // CTHREADWORKER_H
