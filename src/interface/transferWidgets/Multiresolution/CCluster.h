#ifndef CCLUSTER_H
#define CCLUSTER_H

#include <vector>
#include <algorithm>

#include <QImage>
#include <QtGui>

#include "../../../scene/CVolume.h"
#include "../../CMessageBox.h"
#include "CVoxel.h"

#if defined(WIN32) && defined(_DEBUG)
   #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
   #define new DEBUG_NEW
#endif

#define SIGN(a, b) ( (b) < 0 ? -fabs(a) : fabs(a) )

namespace nmMultiresolution
{
	class CGrahamScan
	{
		public:
			CGrahamScan() {};
			~CGrahamScan() {};

			void partition_points();
			void add(int x, int y) { list.push_back( std::make_pair(x,y) ); };
			void build_hull();
			std::pair<int,int> getPoint(int index);
			int size() { return (lower_hull.size() + upper_hull.size() - 2); };
	private:
			std::vector<std::pair<int, int> > list, upper_partition_points, lower_partition_points, lower_hull, upper_hull;
			std::pair<int, int> left, right;

			void build_half_hull(std::vector<std::pair<int,int>> input, std::vector<std::pair<int,int>> &output, int factor );
			static int direction( std::pair<int,int> p0, std::pair<int,int> p1, std::pair<int,int> p2 )
			{
				return ( (p0.first - p1.first ) * (p2.second - p1.second ) ) - ( (p2.first - p1.first ) * (p0.second - p1.second ) );
			}
	};

	class CCluster
	{
		public:
			CCluster() { error = 255; value = 0; volume = NULL; };
			CCluster(CVolume *, char, QImage &);
			~CCluster() { destroy(); };

			void computeCenter();
			void computeCenterMarchingClusters();
			void split(QList<CVoxel<unsigned char> *> *, QList<CVoxel<unsigned char> *> *);
			void destroy();
			void setList(QList<CVoxel<unsigned char> *> list) { this->voxels = list; };
			int sizeOfCluster() { return voxels.size(); };
			void setVolume(CVolume *vol) { this->volume = vol; };
			CVolume *getVolume() { return this->volume; };
			void clearVoxels() { for(int i = 0; i < voxels.size(); i++) delete voxels.takeFirst(); voxels.clear(); };
			unsigned short/*char*/ getError() { return this->error; };
			CVoxel<float> getCenter() { return this->center; };
			unsigned short/*char*/ getValue() { return this->value; };

			// Convex hull of cluster
			QList<CVoxel<int> *> convexhull;
			double **sys,*back;
		private:
			// The center of the cluster has x, y, and z coordinates
			CVoxel<float> center;

			// The error of the entire cluster
			unsigned /*char*/short error;
			
			// The value of the cluster center
			unsigned /*char*/short value;

			// All voxels of the cluster
			QList<CVoxel<unsigned char> *> voxels;

			// Volume to which the cluter belongs. This pointer is employed to obtain voxel intensities from the volume
			CVolume *volume;

			// Eigenbase
			float eigenBase[3];

			// Private methods
			void covcol(float **data, int n, int m, float **symmat);
			float **matrix(int, int);
			float *vector(int);
			void tqli(float d[], float e[], int n, float **z);
			void tred2(float **a, int n, float *d, float *e);
			void free_vector(float *v, int n);
			void free_matrix(float **mat, int n, int m);


			void solution();
			double evaluate(int);
			void result();
			int consistent();
			int nun,neq;
			int moreeq;



	};
}

#endif