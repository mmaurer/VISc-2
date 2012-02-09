#include "CThreadWorker.h"

#include <QThread>

#include "../scene/CVolume.h"

void QThreadWorker::cluster()
{
   computeClusters();
}

void QThreadWorker::computeClusters()
{
   // Zero cluster centers
   m_qlCpDensity.clear();
   m_qlCnDensity.clear();

   // Create initialNumClusters number of initial clusters
   srand(1);
   for (int i = 0; i < m_numClusters; i++)
   {
      // Int clusters
      intCluster newCluster;
      newCluster.center = ( 255.0 / float(m_numClusters) ) * i;
      newCluster.totalNumberElements = 0;
      int r = (rand() / float(RAND_MAX)) * 255;
      int g = (rand() / float(RAND_MAX)) * 255;
      int b = (rand() / float(RAND_MAX)) * 255;
      newCluster.color = QColor(r, g, b);

      m_qlCpDensity << newCluster;
      m_qlCnDensity << newCluster;
      m_qlCpUp << newCluster;
      m_qlCnUp << newCluster;
      m_qlCpDown << newCluster;
      m_qlCnDown << newCluster;

      // Vector clusters
      vecCluster newVCluster;
      newVCluster.center = CVector((1.0 / float(m_numClusters)) * i,
         (1.0 / float(m_numClusters)) * i,
         (1.0 / float(m_numClusters)) * i);
      newVCluster.totalNumberElements = 0;
      newVCluster.color = QColor(r, g, b);

      m_qlCpFirstDerivative << newVCluster;
      m_qlCnFirstDerivative << newVCluster;
      m_qlCpSecondDerivative << newVCluster;
      m_qlCnSecondDerivative << newVCluster;
   }

   emit progressUpdate(0);

   int nclusters = m_numClusters;

   for (int n = 0; n < m_numIterations; n++)
   {
      emit progressUpdate(n);
      for (int i = 0; i < nclusters; i++)
      {
         m_qlCnDensity[i].center = 0;
         m_qlCnDensity[i].totalNumberElements = 0;
         m_qlCnUp[i].center = 0;
         m_qlCnUp[i].totalNumberElements = 0;
         m_qlCnDown[i].center = 0;
         m_qlCnDown[i].totalNumberElements = 0;

         m_qlCnFirstDerivative[i].center.clear();
         m_qlCnFirstDerivative[i].totalNumberElements = 0;
         m_qlCnSecondDerivative[i].center.clear();
         m_qlCnSecondDerivative[i].totalNumberElements = 0;
      }

      for (int x = 0; x < 50/*m_localVolumePtr->getWidth()*/; x++)
      {
         for (int y = 0; y < 50/*m_localVolumePtr->getHeight()*/; y++)
         {
            for (int z = 0; z < 50/*m_localVolumePtr->getDepth()*/; z++)
            {
               emit terminate(m_bTerminate);
               if  (m_bTerminate)
                  return;

               // Density =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
               int value = m_localVolumePtr->getVoxel(x, y, z);
               computeDensityClusters(nclusters, value);
               // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

               // First derivative =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
               int valuex1 = m_localVolumePtr->getVoxel(x-1, y, z);
               int valuex2 = m_localVolumePtr->getVoxel(x+1, y, z);
               int valuey1 = m_localVolumePtr->getVoxel(x, y+1, z);
               int valuey2 = m_localVolumePtr->getVoxel(x, y-1, z);
               int valuez1 = m_localVolumePtr->getVoxel(x, y, z+1);
               int valuez2 = m_localVolumePtr->getVoxel(x, y, z-1);
               CVector gradient(valuex2 - valuex1, valuey2 - valuey1, valuez2 - valuez1);
               gradient.normalize();

               computeFirstDerivativeClusters(nclusters, gradient);
               // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

               // Up =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-
               CVector newPosition = gradient + CPoint(x, y, z);
               value = int( m_localVolumePtr->getVoxel(newPosition) );

               computeUpClusters(nclusters, value);
               // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

               // Down =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-
               gradient.invert();
               newPosition = gradient - CPoint(x, y, z);
               value = int( m_localVolumePtr->getVoxel(newPosition) );

               computeDownClusters(nclusters, value);
               // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

               // Second derivative =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
               // X Direction
               int x1 = m_localVolumePtr->getVoxel(x-2, y, z);
               int x2 = m_localVolumePtr->getVoxel(x+2, y, z);
               valuex1 = value - x1;
               valuex2 = x2 - value;

               // Y Direction
               int y1 = m_localVolumePtr->getVoxel(x, y-2, z);
               int y2 = m_localVolumePtr->getVoxel(x, y+2, z);
               valuey1 = value - y1;
               valuey2 = y2 - value;

               // Z Direction
               int z1 = m_localVolumePtr->getVoxel(x, y, z-2);
               int z2 = m_localVolumePtr->getVoxel(x, y, z+2);
               valuez1 = value - z1;
               valuez2 = z2 - value;

               gradient.set(valuex2 - valuex1, valuey2 - valuey1, valuez2 - valuez1);
               gradient.normalize();

               computeSecondDerivativeClusters(nclusters, gradient);
               // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
            }
         }
      }

      for (int i = 0; i < nclusters; i++)
      {
         m_qlCnDensity[i].center = m_qlCnDensity[i].center / float(m_qlCnDensity[i].totalNumberElements);
         m_qlCnUp[i].center = m_qlCnUp[i].center / float(m_qlCnUp[i].totalNumberElements);
         m_qlCnDown[i].center = m_qlCnDown[i].center / float(m_qlCnDown[i].totalNumberElements);
         m_qlCnFirstDerivative[i].center = m_qlCnFirstDerivative[i].center / float(m_qlCnFirstDerivative[i].totalNumberElements);
         m_qlCnFirstDerivative[i].center.normalize();
         m_qlCnSecondDerivative[i].center = m_qlCnSecondDerivative[i].center / float(m_qlCnSecondDerivative[i].totalNumberElements);
         m_qlCnSecondDerivative[i].center.normalize();
      }

      m_qlCpDensity = m_qlCnDensity;
      m_qlCpUp = m_qlCnUp;
      m_qlCpDown = m_qlCnDown;
      m_qlCpFirstDerivative = m_qlCnFirstDerivative;
      m_qlCpSecondDerivative = m_qlCnSecondDerivative;
   }
   // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
   //if ( m_qtwVisibleItems->topLevelItem(0) != NULL )
   //{
   //   updateTransferFunction( m_qtwVisibleItems->topLevelItem(0) );
   //   updateVisibleItems();
   //   m_qtwVisibleItems->topLevelItem(0)->setCheckState(0, Qt::Checked);
   //}

   // Prepare the interface for new computation
   emit progressUpdate(0);
}
void QThreadWorker::computeFirstDerivativeClusters(int nclusters, CVector &gradient)
{
   double distance = INT_MAX;
   int index = 0;
   for (int i = 0; i < nclusters; i++)
   {
      double currentDistance = gradient.convertToPoint().distanceToPoint( m_qlCpFirstDerivative[i].center.convertToPoint() );

      if (currentDistance < distance)
      {
         index = i;
         distance = currentDistance;
      }
   }
   m_qlCnFirstDerivative[index].center = m_qlCnFirstDerivative[index].center + gradient;
   m_qlCnFirstDerivative[index].totalNumberElements++;
}
void QThreadWorker::computeSecondDerivativeClusters(int nclusters, CVector &gradient)
{
   double distance = INT_MAX;
   int index = 0;
   for (int i = 0; i < nclusters; i++)
   {
      double currentDistance = gradient.convertToPoint().distanceToPoint( m_qlCpSecondDerivative[i].center.convertToPoint() );

      if (currentDistance < distance)
      {
         index = i;
         distance = currentDistance;
      }
   }
   m_qlCnSecondDerivative[index].center = m_qlCnSecondDerivative[index].center + gradient;
   m_qlCnSecondDerivative[index].totalNumberElements++;
}
void QThreadWorker::computeDensityClusters(int nclusters, int value)
{
   int distance = INT_MAX;
   int index = 0;
   for (int i = 0; i < nclusters; i++)
   {
      int currentDistance = abs(value - m_qlCpDensity[i].center);

      if (currentDistance < distance)
      {
         index = i;
         distance = currentDistance;
      }
   }
   m_qlCnDensity[index].center += value;
   m_qlCnDensity[index].totalNumberElements++;
}
void QThreadWorker::computeUpClusters(int nclusters, int value)
{
   int distance = INT_MAX;
   int index = 0;
   for (int i = 0; i < nclusters; i++)
   {
      int currentDistance = abs(value - m_qlCnUp[i].center);

      if (currentDistance < distance)
      {
         index = i;
         distance = currentDistance;
      }
   }
   m_qlCnUp[index].center += value;
   m_qlCnUp[index].totalNumberElements++;
}
void QThreadWorker::computeDownClusters(int nclusters, int value)
{
   int distance = INT_MAX;
   int index = 0;
   for (int i = 0; i < nclusters; i++)
   {
      int currentDistance = abs(value - m_qlCnDown[i].center);

      if (currentDistance < distance)
      {
         index = i;
         distance = currentDistance;
      }
   }
   m_qlCnDown[index].center += value;
   m_qlCnDown[index].totalNumberElements++;
}
