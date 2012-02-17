#include "CBasicClusteringVisualizer.h"

#include <QMessageBox>

#include "..\..\scene\CVolume.h"

CBasicClusteringVisualizer::CBasicClusteringVisualizer() :
CBasicTransferWidget(tr(""), tr("Basic Clustering Visualizer"), VISc::twoNone, Qt::RightDockWidgetArea),
   m_bTerminate(false),
   m_bChanged(true),
   m_bDirty(false),
   m_qiTransferFunctionImage(256, 1, QImage::Format_ARGB32),
   m_localVolumePtr(NULL),
   m_qvlMainLayout( new QVBoxLayout() ),
   m_qvlTWidgetLayout( new QVBoxLayout() ),
   m_saInternalPanel( new QScrollArea() ),
   m_qtwVisibleItems( new QTreeWidget() ),
   m_qpbComputationProgress( new QProgressBar() ),
   m_qsbNumIterations( new QSpinBox() ),
   m_qsbInitialNumClusters( new QSpinBox() ),
   m_qpbCalculate( new QPushButton(tr("Calculate")) ),
   m_qlNumIterationsLabel( new QLabel(tr("Number of Iterations")) ),
   m_qlNumInitialClustersLabel( new QLabel(tr("Initial Number of clusters")) ),
   m_tw(m_qlCpDensity, m_qlCpUp, m_qlCpDown, m_qlCpFirstDerivative, m_qlCpSecondDerivative)
{
   // Local Volume
   //m_localVolumePtr = new CVolume();

   m_saInternalPanel->setLayout(m_qvlMainLayout.data());
   m_saInternalPanel->setFrameStyle(QFrame::NoFrame);
   m_saInternalPanel->setContentsMargins(0, 0, 0, 0);
   m_qvlTWidgetLayout->addWidget(m_saInternalPanel.data());

   // Tree Widget
   QStringList headers;
   headers << tr("Metric") << tr("Value") << tr("Color");

   m_qtwVisibleItems->setColumnCount(3);
   connect(m_qtwVisibleItems.data(), SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(itemClicked(QTreeWidgetItem *, int)));
   connect(m_qtwVisibleItems.data(), SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));
   m_qtwVisibleItems->setHeaderLabels(headers);
   m_qvlMainLayout->addWidget(m_qtwVisibleItems.data());

   // Progress bar
   m_qvlMainLayout->addWidget(m_qpbComputationProgress.data());

   // Options
   // Number of iterations
   m_qsbNumIterations->setValue(100);
   m_qsbNumIterations->setMinimum(5);
   m_qsbNumIterations->setMaximum(1000);

   // Initial number of clusters
   m_qsbInitialNumClusters->setValue(5);
   m_qsbInitialNumClusters->setMinimum(1);
   m_qsbInitialNumClusters->setMaximum(50);

   m_qvlMainLayout->addWidget(m_qlNumIterationsLabel.data());
   m_qvlMainLayout->addWidget(m_qsbNumIterations.data());
   m_qvlMainLayout->addWidget(m_qlNumInitialClustersLabel.data());
   m_qvlMainLayout->addWidget(m_qsbInitialNumClusters.data());

   // Calculate Button
   m_qpbCalculate->setEnabled(false);
   connect(m_qpbCalculate.data(), SIGNAL(clicked(bool)), this, SLOT(clicked(bool)));
   m_qvlMainLayout->addWidget(m_qpbCalculate.data());

   m_qvlMainLayout->addStretch(1);
   this->setLayout(m_qvlTWidgetLayout.data());

   updateVisibleItems();
}

// Inherited methos
QImage CBasicClusteringVisualizer::getTransferFunction(int)
{
   return m_qiTransferFunctionImage;
}

void CBasicClusteringVisualizer::computeHistogram(int volumeIndex, bool)
{
   if (m_volumeListPtr != NULL)
   {
      if (volumeIndex < m_volumeListPtr->count())
      {
         m_currentVolumeIndex = volumeIndex;
         m_localVolumePtr = m_volumeListPtr->at(m_currentVolumeIndex).data();
         m_qpbCalculate->setEnabled(true);
      }
   }
}
// end of inherited methods

// Methods
void CBasicClusteringVisualizer::updateVisibleItems()
{
   m_qtwVisibleItems->clear();

   QTreeWidgetItem* densityItem = new QTreeWidgetItem(m_qtwVisibleItems.data());
   densityItem->setText(0, tr("Density"));
   densityItem->setSizeHint(0, QSize(0, 16));
   densityItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
   densityItem->setCheckState(0, Qt::Unchecked);

   QTreeWidgetItem* upDensityItem = new QTreeWidgetItem(m_qtwVisibleItems.data());
   upDensityItem->setText(0, tr("Up Density"));
   upDensityItem->setSizeHint(0, QSize(0, 16));
   upDensityItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
   upDensityItem->setCheckState(0, Qt::Unchecked);

   QTreeWidgetItem* downDensityItem = new QTreeWidgetItem(m_qtwVisibleItems.data());
   downDensityItem->setText(0, tr("Down Density"));
   downDensityItem->setSizeHint(0, QSize(0, 16));
   downDensityItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
   downDensityItem->setCheckState(0, Qt::Unchecked);

   QTreeWidgetItem* firstDerivativeItem = new QTreeWidgetItem(m_qtwVisibleItems.data());
   firstDerivativeItem->setText(0, tr("First Derivative"));
   firstDerivativeItem->setSizeHint(0, QSize(0, 16));
   firstDerivativeItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
   firstDerivativeItem->setCheckState(0, Qt::Unchecked);

   QTreeWidgetItem* secondDerivativeItem = new QTreeWidgetItem(m_qtwVisibleItems.data());
   secondDerivativeItem->setText(0, tr("Second Derivative"));
   secondDerivativeItem->setSizeHint(0, QSize(0, 16));
   secondDerivativeItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
   secondDerivativeItem->setCheckState(0, Qt::Unchecked);

   for ( int i = 0; i < m_qlCpDensity.count(); i++)
   {
      QTreeWidgetItem *dcluster = new QTreeWidgetItem(densityItem);
      dcluster->setText(0, tr("%1").arg(i));
      dcluster->setText(1, tr("%1").arg(m_qlCpDensity[i].center));
      dcluster->setText(2, tr("(%1; %2; %3)").arg(m_qlCpDensity[i].color.red()).arg(m_qlCpDensity[i].color.green()).arg(m_qlCpDensity[i].color.blue()));
      dcluster->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
      dcluster->setCheckState(0, Qt::Checked);
      dcluster->setForeground(2, QBrush(m_qlCpDensity[i].color));

      QTreeWidgetItem *fdcluster = new QTreeWidgetItem(firstDerivativeItem);
      fdcluster->setText(0, tr("%1").arg(i));
      fdcluster->setText(1, tr("(%1; %2; %3)").arg(m_qlCpFirstDerivative[i].center.getX()).arg(m_qlCpFirstDerivative[i].center.getY()).arg(m_qlCpFirstDerivative[i].center.getZ()) );
      fdcluster->setText(2, tr("(%1; %2; %3)").arg(m_qlCpFirstDerivative[i].color.red()).arg(m_qlCpFirstDerivative[i].color.green()).arg(m_qlCpFirstDerivative[i].color.blue()));
      fdcluster->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
      fdcluster->setCheckState(0, Qt::Checked);
      fdcluster->setForeground(2, QBrush(m_qlCpFirstDerivative[i].color));

      QTreeWidgetItem *sdcluster = new QTreeWidgetItem(secondDerivativeItem);
      sdcluster->setText(0, tr("%1").arg(i));
      sdcluster->setText(1, tr("(%1; %2; %3)").arg(m_qlCpSecondDerivative[i].center.getX()).arg(m_qlCpSecondDerivative[i].center.getY()).arg(m_qlCpSecondDerivative[i].center.getZ()) );
      sdcluster->setText(2, tr("(%1; %2; %3)").arg(m_qlCpSecondDerivative[i].color.red()).arg(m_qlCpSecondDerivative[i].color.green()).arg(m_qlCpSecondDerivative[i].color.blue()));
      sdcluster->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
      sdcluster->setCheckState(0, Qt::Checked);
      sdcluster->setForeground(2, QBrush(m_qlCpSecondDerivative[i].color));

      QTreeWidgetItem *udcluster = new QTreeWidgetItem(upDensityItem);
      udcluster->setText(0, tr("%1").arg(i));
      udcluster->setText(1, tr("%1").arg(m_qlCpUp[i].center));
      udcluster->setText(2, tr("(%1; %2; %3)").arg(m_qlCpUp[i].color.red()).arg(m_qlCpUp[i].color.green()).arg(m_qlCpUp[i].color.blue()));
      udcluster->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
      udcluster->setCheckState(0, Qt::Checked);
      udcluster->setForeground(2, QBrush(m_qlCpUp[i].color));

      QTreeWidgetItem *ddcluster = new QTreeWidgetItem(downDensityItem);
      ddcluster->setText(0, tr("%1").arg(i));
      ddcluster->setText(1, tr("%1").arg(m_qlCpDown[i].center));
      ddcluster->setText(2, tr("(%1; %2; %3)").arg(m_qlCpDown[i].color.red()).arg(m_qlCpDown[i].color.green()).arg(m_qlCpDown[i].color.blue()));
      ddcluster->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
      ddcluster->setCheckState(0, Qt::Checked);
      ddcluster->setForeground(2, QBrush(m_qlCpDown[i].color));
   }
}
//void CBasicClusteringVisualizer::computeClusters()
//{
//   // Initialize structures
//   m_qpbCalculate->setEnabled(false);
//
//   // Zero cluster centers
//   m_qlCpDensity.clear();
//   m_qlCnDensity.clear();
//
//   // Create initialNumClusters number of initial clusters
//   srand(1);
//   for (int i = 0; i < m_qsbInitialNumClusters->value(); i++)
//   {
//      // Int clusters
//      intCluster newCluster;
//      newCluster.center = ( 255.0 / float(m_qsbInitialNumClusters->value()) ) * i;
//      newCluster.totalNumberElements = 0;
//      int r = (rand() / float(RAND_MAX)) * 255;
//      int g = (rand() / float(RAND_MAX)) * 255;
//      int b = (rand() / float(RAND_MAX)) * 255;
//      newCluster.color = QColor(r, g, b);
//
//      m_qlCpDensity << newCluster;
//      m_qlCnDensity << newCluster;
//      m_qlCpUp << newCluster;
//      m_qlCnUp << newCluster;
//      m_qlCpDown << newCluster;
//      m_qlCnDown << newCluster;
//
//      // Vector clusters
//      vecCluster newVCluster;
//      newVCluster.center = CVector((1.0 / float(m_qsbInitialNumClusters->value())) * i,
//         (1.0 / float(m_qsbInitialNumClusters->value())) * i,
//         (1.0 / float(m_qsbInitialNumClusters->value())) * i);
//      newVCluster.totalNumberElements = 0;
//      newVCluster.color = QColor(r, g, b);
//
//      m_qlCpFirstDerivative << newVCluster;
//      m_qlCnFirstDerivative << newVCluster;
//      m_qlCpSecondDerivative << newVCluster;
//      m_qlCnSecondDerivative << newVCluster;
//   }
//   m_qpbComputationProgress->setMaximum( m_qsbNumIterations->value() );
//   m_qpbComputationProgress->setValue(0);
//
//   int nclusters = m_qsbInitialNumClusters->value();
//
//   for (int n = 0; n < m_qsbNumIterations->value(); n++)
//   {
//      m_qpbComputationProgress->setValue(n);
//      for (int i = 0; i < nclusters; i++)
//      {
//         m_qlCnDensity[i].center = 0;
//         m_qlCnDensity[i].totalNumberElements = 0;
//         m_qlCnUp[i].center = 0;
//         m_qlCnUp[i].totalNumberElements = 0;
//         m_qlCnDown[i].center = 0;
//         m_qlCnDown[i].totalNumberElements = 0;
//
//         m_qlCnFirstDerivative[i].center.clear();
//         m_qlCnFirstDerivative[i].totalNumberElements = 0;
//         m_qlCnSecondDerivative[i].center.clear();
//         m_qlCnSecondDerivative[i].totalNumberElements = 0;
//      }
//
//      for (int x = 0; x < m_localVolumePtr->getWidth(); x++)
//      {
//         for (int y = 0; y < m_localVolumePtr->getHeight(); y++)
//         {
//            for (int z = 0; z < m_localVolumePtr->getDepth(); z++)
//            {
//               // Density =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//               int value = m_localVolumePtr->getVoxel(x, y, z);
//               computeDensityClusters(nclusters, value);
//               // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
//               // First derivative =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//               int valuex1 = m_localVolumePtr->getVoxel(x-1, y, z);
//               int valuex2 = m_localVolumePtr->getVoxel(x+1, y, z);
//               int valuey1 = m_localVolumePtr->getVoxel(x, y+1, z);
//               int valuey2 = m_localVolumePtr->getVoxel(x, y-1, z);
//               int valuez1 = m_localVolumePtr->getVoxel(x, y, z+1);
//               int valuez2 = m_localVolumePtr->getVoxel(x, y, z-1);
//               CVector gradient(valuex2 - valuex1, valuey2 - valuey1, valuez2 - valuez1);
//               gradient.normalize();
//
//               computeFirstDerivativeClusters(nclusters, gradient);
//               // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
//               // Up =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-
//               CVector newPosition = gradient + CPoint(x, y, z);
//               value = int( m_localVolumePtr->getVoxel(newPosition) );
//
//               computeUpClusters(nclusters, value);
//               // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
//               // Down =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-
//               gradient.invert();
//               newPosition = gradient - CPoint(x, y, z);
//               value = int( m_localVolumePtr->getVoxel(newPosition) );
//
//               computeDownClusters(nclusters, value);
//               // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
//               // Second derivative =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//               // X Direction
//               int x1 = m_localVolumePtr->getVoxel(x-2, y, z);
//               int x2 = m_localVolumePtr->getVoxel(x+2, y, z);
//               valuex1 = value - x1;
//               valuex2 = x2 - value;
//
//               // Y Direction
//               int y1 = m_localVolumePtr->getVoxel(x, y-2, z);
//               int y2 = m_localVolumePtr->getVoxel(x, y+2, z);
//               valuey1 = value - y1;
//               valuey2 = y2 - value;
//
//               // Z Direction
//               int z1 = m_localVolumePtr->getVoxel(x, y, z-2);
//               int z2 = m_localVolumePtr->getVoxel(x, y, z+2);
//               valuez1 = value - z1;
//               valuez2 = z2 - value;
//
//               gradient.set(valuex2 - valuex1, valuey2 - valuey1, valuez2 - valuez1);
//               gradient.normalize();
//
//               computeSecondDerivativeClusters(nclusters, gradient);
//               // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//            }
//         }
//      }
//
//      for (int i = 0; i < nclusters; i++)
//      {
//         m_qlCnDensity[i].center = m_qlCnDensity[i].center / float(m_qlCnDensity[i].totalNumberElements);
//         m_qlCnUp[i].center = m_qlCnUp[i].center / float(m_qlCnUp[i].totalNumberElements);
//         m_qlCnDown[i].center = m_qlCnDown[i].center / float(m_qlCnDown[i].totalNumberElements);
//         m_qlCnFirstDerivative[i].center = m_qlCnFirstDerivative[i].center / float(m_qlCnFirstDerivative[i].totalNumberElements);
//         m_qlCnFirstDerivative[i].center.normalize();
//         m_qlCnSecondDerivative[i].center = m_qlCnSecondDerivative[i].center / float(m_qlCnSecondDerivative[i].totalNumberElements);
//         m_qlCnSecondDerivative[i].center.normalize();
//      }
//
//      m_qlCpDensity = m_qlCnDensity;
//      m_qlCpUp = m_qlCnUp;
//      m_qlCpDown = m_qlCnDown;
//      m_qlCpFirstDerivative = m_qlCnFirstDerivative;
//      m_qlCpSecondDerivative = m_qlCnSecondDerivative;
//   }
//   // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//   if ( m_qtwVisibleItems->topLevelItem(0) != NULL )
//   {
//      updateTransferFunction( m_qtwVisibleItems->topLevelItem(0) );
//      updateVisibleItems();
//      m_qtwVisibleItems->topLevelItem(0)->setCheckState(0, Qt::Checked);
//   }
//
//   // Prepare the interface for new computation
//   m_qpbComputationProgress->setValue(0);
//   m_qpbCalculate->setEnabled(true);
//}
//void CBasicClusteringVisualizer::computeFirstDerivativeClusters(int nclusters, CVector &gradient)
//{
//   double distance = INT_MAX;
//   int index = 0;
//   for (int i = 0; i < nclusters; i++)
//   {
//      double currentDistance = gradient.convertToPoint().distanceToPoint( m_qlCpFirstDerivative[i].center.convertToPoint() );
//
//      if (currentDistance < distance)
//      {
//         index = i;
//         distance = currentDistance;
//      }
//   }
//   m_qlCnFirstDerivative[index].center = m_qlCnFirstDerivative[index].center + gradient;
//   m_qlCnFirstDerivative[index].totalNumberElements++;
//}
//void CBasicClusteringVisualizer::computeSecondDerivativeClusters(int nclusters, CVector &gradient)
//{
//   double distance = INT_MAX;
//   int index = 0;
//   for (int i = 0; i < nclusters; i++)
//   {
//      double currentDistance = gradient.convertToPoint().distanceToPoint( m_qlCpSecondDerivative[i].center.convertToPoint() );
//
//      if (currentDistance < distance)
//      {
//         index = i;
//         distance = currentDistance;
//      }
//   }
//   m_qlCnSecondDerivative[index].center = m_qlCnSecondDerivative[index].center + gradient;
//   m_qlCnSecondDerivative[index].totalNumberElements++;
//}
//void CBasicClusteringVisualizer::computeDensityClusters(int nclusters, int value)
//{
//   int distance = INT_MAX;
//   int index = 0;
//   for (int i = 0; i < nclusters; i++)
//   {
//      int currentDistance = abs(value - m_qlCpDensity[i].center);
//
//      if (currentDistance < distance)
//      {
//         index = i;
//         distance = currentDistance;
//      }
//   }
//   m_qlCnDensity[index].center += value;
//   m_qlCnDensity[index].totalNumberElements++;
//}
//void CBasicClusteringVisualizer::computeUpClusters(int nclusters, int value)
//{
//   int distance = INT_MAX;
//   int index = 0;
//   for (int i = 0; i < nclusters; i++)
//   {
//      int currentDistance = abs(value - m_qlCnUp[i].center);
//
//      if (currentDistance < distance)
//      {
//         index = i;
//         distance = currentDistance;
//      }
//   }
//   m_qlCnUp[index].center += value;
//   m_qlCnUp[index].totalNumberElements++;
//}
//void CBasicClusteringVisualizer::computeDownClusters(int nclusters, int value)
//{
//   int distance = INT_MAX;
//   int index = 0;
//   for (int i = 0; i < nclusters; i++)
//   {
//      int currentDistance = abs(value - m_qlCnDown[i].center);
//
//      if (currentDistance < distance)
//      {
//         index = i;
//         distance = currentDistance;
//      }
//   }
//   m_qlCnDown[index].center += value;
//   m_qlCnDown[index].totalNumberElements++;
//}

void CBasicClusteringVisualizer::updateTransferFunction(QTreeWidgetItem *parent)
{
   // No problem if we use cpDensity list for all metrics. Since we filled the list with the same colors for every cluster,
   // there won't be any difference between colors from any of the lists.
   for (int i = 0; i < 256; i++)
      m_qiTransferFunctionImage.setPixel(i, 0, qRgba(0, 0, 0, 0));

   for (int i = 0; i < parent->childCount(); i++)
   {
      if (parent->child(i)->checkState(0) == Qt::Checked)
         m_qiTransferFunctionImage.setPixel(i * 10, 0, qRgba(m_qlCpDensity[i].color.red(), m_qlCpDensity[i].color.green(), m_qlCpDensity[i].color.blue(), 255));
   }
}
void CBasicClusteringVisualizer::checkOne(QTreeWidgetItem *item)
{
   for (int i = 0; i < m_qtwVisibleItems->topLevelItemCount() ; i++)
      m_qtwVisibleItems->topLevelItem(i)->setCheckState(0, Qt::Unchecked);

   item->setCheckState(0, Qt::Checked);
}
// end of methods

// Signals
void CBasicClusteringVisualizer::updateVolumeSecondDerivative()
{
   for (int x = 0; x < m_localVolumePtr->getWidth(); x++)
      for (int y = 0; y < m_localVolumePtr->getHeight(); y++)
         for (int z = 0; z < m_localVolumePtr->getDepth(); z++)
         {
            int value = m_localVolumePtr->getVoxel(x, y, z);
            int x1 = m_localVolumePtr->getVoxel(x-2, y, z);
            int x2 = m_localVolumePtr->getVoxel(x+2, y, z);
            int valuex1 = value - x1;
            int valuex2 = x2 - value;

            // Y Direction
            int y1 = m_localVolumePtr->getVoxel(x, y-2, z);
            int y2 = m_localVolumePtr->getVoxel(x, y+2, z);
            int valuey1 = value - y1;
            int valuey2 = y2 - value;

            // Z Direction
            int z1 = m_localVolumePtr->getVoxel(x, y, z-2);
            int z2 = m_localVolumePtr->getVoxel(x, y, z+2);
            int valuez1 = value - z1;
            int valuez2 = z2 - value;

            CVector gradient(valuex2 - valuex1, valuey2 - valuey1, valuez2 - valuez1);
            gradient.normalize();

            double distance = INT_MAX;
            int index = 0;
            for (int i = 0; i < m_qsbInitialNumClusters->value(); i++)
            {
               double currentDistance = gradient.convertToPoint().distanceToPoint( m_qlCpSecondDerivative[i].center.convertToPoint() );

               if (currentDistance < distance)
               {
                  index = i;
                  distance = currentDistance;
               }
            }
            m_volumeListPtr->at(0)->setVoxel(x, y, z, index * 10);
         }
}
void CBasicClusteringVisualizer::updateVolumeFirstDerivative()
{
   for (int x = 0; x < m_localVolumePtr->getWidth(); x++)
      for (int y = 0; y < m_localVolumePtr->getHeight(); y++)
         for (int z = 0; z < m_localVolumePtr->getDepth(); z++)
         {
            int valuex1 = m_localVolumePtr->getVoxel(x-1, y, z);
            int valuex2 = m_localVolumePtr->getVoxel(x+1, y, z);
            int valuey1 = m_localVolumePtr->getVoxel(x, y+1, z);
            int valuey2 = m_localVolumePtr->getVoxel(x, y-1, z);
            int valuez1 = m_localVolumePtr->getVoxel(x, y, z+1);
            int valuez2 = m_localVolumePtr->getVoxel(x, y, z-1);
            CVector gradient(valuex2 - valuex1, valuey2 - valuey1, valuez2 - valuez1);
            gradient.normalize();

            double distance = INT_MAX;
            int index = 0;
            for (int i = 0; i < m_qsbInitialNumClusters->value(); i++)
            {
               double currentDistance = gradient.convertToPoint().distanceToPoint( m_qlCpFirstDerivative[i].center.convertToPoint() );

               if (currentDistance < distance)
               {
                  index = i;
                  distance = currentDistance;
               }
            }
            m_volumeListPtr->at(0)->setVoxel(x, y, z, index * 10);
         }
}
void CBasicClusteringVisualizer::updateVolumeDownDensity()
{
   for (int x = 0; x < m_localVolumePtr->getWidth(); x++)
      for (int y = 0; y < m_localVolumePtr->getHeight(); y++)
         for (int z = 0; z < m_localVolumePtr->getDepth(); z++)
         {
            int valuex1 = m_localVolumePtr->getVoxel(x-1, y, z);
            int valuex2 = m_localVolumePtr->getVoxel(x+1, y, z);
            int valuey1 = m_localVolumePtr->getVoxel(x, y+1, z);
            int valuey2 = m_localVolumePtr->getVoxel(x, y-1, z);
            int valuez1 = m_localVolumePtr->getVoxel(x, y, z+1);
            int valuez2 = m_localVolumePtr->getVoxel(x, y, z-1);
            CVector gradient(valuex2 - valuex1, valuey2 - valuey1, valuez2 - valuez1);
            gradient.normalize();
            gradient.invert();

            CVector newPosition = gradient + CPoint(x, y, z);
            int value = int( m_localVolumePtr->getVoxel(newPosition) );

            int distance = INT_MAX;
            int index = 0;
            for (int i = 0; i < m_qsbInitialNumClusters->value(); i++)
            {
               int currentDistance = abs(value - m_qlCpUp[i].center);

               if (currentDistance < distance)
               {
                  index = i;
                  distance = currentDistance;
               }
            }
            m_volumeListPtr->at(0)->setVoxel(x, y, z, index * 10);
         }
}
void CBasicClusteringVisualizer::updateVolumeUpDensity()
{
   for (int x = 0; x < m_localVolumePtr->getWidth(); x++)
      for (int y = 0; y < m_localVolumePtr->getHeight(); y++)
         for (int z = 0; z < m_localVolumePtr->getDepth(); z++)
         {
            int valuex1 = m_localVolumePtr->getVoxel(x-1, y, z);
            int valuex2 = m_localVolumePtr->getVoxel(x+1, y, z);
            int valuey1 = m_localVolumePtr->getVoxel(x, y+1, z);
            int valuey2 = m_localVolumePtr->getVoxel(x, y-1, z);
            int valuez1 = m_localVolumePtr->getVoxel(x, y, z+1);
            int valuez2 = m_localVolumePtr->getVoxel(x, y, z-1);
            CVector gradient(valuex2 - valuex1, valuey2 - valuey1, valuez2 - valuez1);
            gradient.normalize();

            CVector newPosition = gradient + CPoint(x, y, z);
            int value = int( m_localVolumePtr->getVoxel(newPosition) );

            int distance = INT_MAX;
            int index = 0;
            for (int i = 0; i < m_qsbInitialNumClusters->value(); i++)
            {
               int currentDistance = abs(value - m_qlCpUp[i].center);

               if (currentDistance < distance)
               {
                  index = i;
                  distance = currentDistance;
               }
            }
            m_volumeListPtr->at(0)->setVoxel(x, y, z, index * 10);
         }
}
void CBasicClusteringVisualizer::updateVolumeDensity()
{
   for (int x = 0; x < m_localVolumePtr->getWidth(); x++)
      for (int y = 0; y < m_localVolumePtr->getHeight(); y++)
         for (int z = 0; z < m_localVolumePtr->getDepth(); z++)
         {
            int value = m_localVolumePtr->getVoxel(x, y, z);
            int distance = INT_MAX;
            int index = 0;
            for (int i = 0; i < m_qsbInitialNumClusters->value(); i++)
            {
               int currentDistance = abs(value - m_qlCpDensity[i].center);

               if (currentDistance < distance)
               {
                  index = i;
                  distance = currentDistance;
               }
            }
            m_volumeListPtr->at(0)->setVoxel(x, y, z, index * 10);
         }
}
void CBasicClusteringVisualizer::itemClicked ( QTreeWidgetItem * item, int )
{
   if (item->childCount() < 1)
   {
      if (item->checkState(0) == Qt::Checked)
         item->setCheckState(0, Qt::Unchecked);
      else if (item->checkState(0) == Qt::Unchecked)
         item->setCheckState(0, Qt::Checked);

      if (item->parent()->checkState(0) == Qt::Checked)
      {
         // Update transfer function
         updateTransferFunction(item->parent());
         m_bChanged = true;
      }
   }
   else
   {
      checkOne(item);

      if (item->text(0) == tr("Density"))
         updateVolumeDensity();
      else if (item->text(0) == tr("Up Density"))
         updateVolumeUpDensity();
      else if (item->text(0) == tr("Down Density"))
         updateVolumeDownDensity();
      else if (item->text(0) == tr("First Derivative"))
         updateVolumeFirstDerivative();
      else if (item->text(0) == tr("Second Derivative"))
         updateVolumeSecondDerivative();

      m_bDirty = true;
   }
}

void CBasicClusteringVisualizer::itemDoubleClicked ( QTreeWidgetItem *, int )
{
   QMessageBox::information(this, tr("Info"), tr("Double Clicked"));
}

void CBasicClusteringVisualizer::clicked (bool)
{
   m_qpbCalculate->setEnabled(false);
   m_qpbComputationProgress->setMaximum( m_qsbNumIterations->value() );
   m_qpbComputationProgress->setMaximum(100);

   connect(&m_tw, SIGNAL(terminate(bool&)), this, SLOT(terminateWorker(bool&)), Qt::QueuedConnection);
   connect(&m_tw, SIGNAL(progressUpdate(int)), this, SLOT(progressUpdated(int)), Qt::QueuedConnection);
   connect(&m_tcc, SIGNAL(finished), this, SLOT(clusterComputationFinished), Qt::QueuedConnection);
   m_tw.setNumClusters(m_qsbInitialNumClusters->value());
   m_tw.setNumIterations(m_qsbNumIterations->value());
   m_tw.setVolume(m_localVolumePtr);
   m_tw.moveToThread(&m_tcc);
   m_tcc.start();

   QMetaObject::invokeMethod(&m_tw, "cluster", Qt::QueuedConnection);
}
// end of signals

void CBasicClusteringVisualizer::clusterComputationFinished()
{
   if ( m_qtwVisibleItems->topLevelItem(0) != NULL )
   {
      updateTransferFunction( m_qtwVisibleItems->topLevelItem(0) );
      updateVisibleItems();
      m_qtwVisibleItems->topLevelItem(0)->setCheckState(0, Qt::Checked);
   }


   QMessageBox::information(this, tr("Info"), tr("Cluster computation finished."));
   m_qpbCalculate->setEnabled(true);
}

void CBasicClusteringVisualizer::progressUpdated(int value)
{
   m_qpbComputationProgress->setValue(value);
}

void CBasicClusteringVisualizer::destroy()
{
   QMetaObject::invokeMethod(&m_tw, "terminate", Qt::QueuedConnection);
   m_bTerminate = true;
   //m_tcc.quit();
   m_tcc.wait();
}
