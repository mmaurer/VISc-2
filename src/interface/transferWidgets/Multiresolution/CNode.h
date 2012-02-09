#ifndef CNODE_H
#define CNODE_H

#include <iostream>

#include <QList>

#include "CVoxel.h"
#include "CCluster.h"

#if defined(WIN32) && defined(_DEBUG)
   #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
   #define new DEBUG_NEW
#endif

namespace nmMultiresolution
{
	enum ETraversalType { ttLevelBased, ttErrorBased };
	enum ERenderMode {rmMarchingClusters, rmNormal };

	template<typename T>
	class CNode
	{
		public:
			CNode() { object = left = right = NULL; };
			CNode(T *pointer) { object = pointer; left = right = NULL; };
			~CNode();

			void generateCBT(int, int, QProgressDialog *);
			void generateCBTMarchingClusters(int level, int minCluster, QProgressDialog *pd);
			void setRight(CNode *l) { this->left = l; };
			void setLeft(CNode *r) { this->right = r; };
			int getMaxError();
			int getDepth(int level);
			int getNumNodes();
			void render(CVolume *volume, ETraversalType ttype, int value, int level);
			void renderMarchingClusters(CVolume *volume, ETraversalType ttype, int value, int level);

		private:
			// Each node holds a cluster
			T *object;

			// Each node has two descendants: one right and one left
			CNode *left, *right;
	};

	template<typename T> CNode<T>::~CNode()
	{
		if (left != NULL)
			delete left;

		if (right != NULL)
			delete right;

		if (object != NULL)
			delete object;
	}

	template<typename T> void CNode<T>::generateCBT(int level, int minCluster, QProgressDialog *pd)
	{
//		CMessageBox::getInstance()->writeMessage(QObject::tr("Level: %1").arg(level), VISc::mtInformation);

		if (object->sizeOfCluster() < minCluster)
		{
			object->clearVoxels();
			return;
		}

		QList<CVoxel<unsigned char> *> left, right;
		object->split(&left, &right);

		if (left.size() >= minCluster && right.size() != 0)
		{
			T *l = new T();
			l->setList(left);
			l->setVolume(object->getVolume());
			l->computeCenter();
			this->left = new CNode(l);
			this->left->generateCBT(level+1, minCluster, pd);
		}
		else
		{
			for (int i = 0; i < left.size(); i++)
				delete left.takeFirst();
		}

		if (right.size() >= minCluster && left.size() != 0)
		{
			T *r = new T();
			r->setList(right);
			r->setVolume(object->getVolume());
			r->computeCenter();
			this->right = new CNode(r);
			this->right->generateCBT(level+1, minCluster, pd);
		}
		else
		{
			for (int i = 0; i < right.size(); i++)
				delete right.takeFirst();
		}
	}

	template<typename T> int CNode<T>::getDepth(int level)
	{
		if (this->left == NULL && right == NULL)
			return level;

		if (right == NULL && left != NULL)
			return left->getDepth(level+1);
		if (right != NULL && left == NULL)
			return right->getDepth(level+1);
			
      return std::max<int>(left->getDepth(level+1), right->getDepth(level+1));
	}

	template<typename T> int CNode<T>::getNumNodes()
	{
		if (this->left == NULL && right == NULL)
			return 1;

		if (right == NULL && left != NULL)
			return left->getNumNodes() + 1;
		if (right != NULL && left == NULL)
			return right->getNumNodes() + 1;
			
		return left->getNumNodes() + right->getNumNodes() + 1;
	}

	template<typename T> int CNode<T>::getMaxError()
	{
		if (left == NULL && right == NULL)
			return object->getError();

		if (right == NULL && left != NULL)
         return std::max<int>(object->getError(), left->getMaxError());
		if (right != NULL && left == NULL)
			return std::max<int>(object->getError(), right->getMaxError());
			
		int winner = std::max<int>(left->getMaxError(), right->getMaxError());
		return std::max<int>(winner, object->getError());
	}

	template<typename T> void CNode<T>::render(CVolume *volume, ETraversalType ttype, int value, int level)
	{
		bool draw = false;
		if (ttype == ttErrorBased)
		{
			//if (((int) object->getError()) > value)
			//	return;

			if (object->getError() < value)
				draw = true;
		}
		else if (ttype == ttLevelBased)
		{
			if (level > value)
				return;

			if (value == level)
				draw = true;
		}

		if ((left == NULL && right == NULL) || draw)
		{
			CVoxel<float> center = object->getCenter();
			volume->setVoxel(center.x, center.y, center.z, (int) object->getValue());
			return;
		}

		if (left != NULL)
			left->render(volume, ttype, value, level+1);
		if (right != NULL)
			right->render(volume, ttype, value, level+1);
	}

	template<typename T> void CNode<T>::renderMarchingClusters(CVolume *volume, ETraversalType ttype, int value, int level)
	{
		if (ttype == ttErrorBased)
		{
			if (((int) object->getError()) < value)
				return;
		}
		else if (ttype == ttLevelBased)
		{
			if (level > value)
				return;
		}

		if (level == value || (left == NULL && right == NULL))
		{
			//float *v;
			//int *nv, *f, *nf;
			//object->ch->get_convex_hull (v, nv, f, nf);

			//glBegin(GL_TRIANGLES);
			//	for (int i = 0; i < *nf; i++)
			//	{
			//		glVertex3f(v[3*f[3*i]], v[3*f[3*i]+1], v[3*f[3*i]+2]);
			//		glVertex3f(v[3*f[3*i+1]], v[3*f[3*i+1]+1], v[3*f[3*i+1]+2]);
			//		glVertex3f(v[3*f[3*i+2]], v[3*f[3*i+2]+1], v[3*f[3*i+2]+2]);
			//	}
			//glEnd();


			glBegin(GL_POLYGON);
				CVoxel<float> center = object->getCenter();
				for (int i = 0; i < object->convexhull.size(); i++)
					glVertex3f(object->convexhull.at(i)->x + center.x, object->convexhull.at(i)->y + center.y, object->convexhull.at(i)->z + center.z);
			glEnd();
		}

		if (left != NULL)
			left->renderMarchingClusters(volume, ttype, value, level+1);
		if (right != NULL)
			right->renderMarchingClusters(volume, ttype, value, level+1);
	}

	// Generates the CBT with Marching Clusters
	template<typename T> void CNode<T>::generateCBTMarchingClusters(int level, int minCluster, QProgressDialog *pd)
	{
		if (object->sizeOfCluster() < minCluster)
		{
			object->clearVoxels();
			return;
		}

		QList<CVoxel<unsigned char> *> left, right;
		object->split(&left, &right);

		if (left.size() >= minCluster && right.size() != 0)
		{
			T *l = new T();
			l->setList(left);
			l->setVolume(object->getVolume());
			l->computeCenterMarchingClusters();
			this->left = new CNode(l);
			this->left->generateCBTMarchingClusters(level+1, minCluster, pd);
		}
		else
		{
			for (int i = 0; i < left.size(); i++)
				delete left.takeFirst();
		}

		if (right.size() >= minCluster && left.size() != 0)
		{
			T *r = new T();
			r->setList(right);
			r->setVolume(object->getVolume());
			r->computeCenterMarchingClusters();
			this->right = new CNode(r);
			this->right->generateCBTMarchingClusters(level+1, minCluster, pd);
		}
		else
		{
			for (int i = 0; i < right.size(); i++)
				delete right.takeFirst();
		}
	}

}

#endif
