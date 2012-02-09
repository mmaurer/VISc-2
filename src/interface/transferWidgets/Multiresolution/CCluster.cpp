#include "CCluster.h"

//#include "../../../algorithms/sysmat.h"

namespace nmMultiresolution
{
	// COMPARISON FUNCTION =============================================
	float xxx, yyy, zzz;
	bool compare_nocase (CVoxel<unsigned char> first, CVoxel<unsigned char> second)
	{
	  float d1, d2;
	  d1 = first.x*first.x+first.y*first.y+first.z*first.z;
	  d2 = second.x*second.x+second.y*second.y+second.z*second.z;

	  if (d1 < d2)
		  return true;

		return false;
	}
	// =================================================================

	// Graham Scan =====================================================
	void CGrahamScan::partition_points()
	{
		/*   Step one in partitioning the points is to sort the raw data */
		std::sort( list.begin(), list.end() );
		
		/* The the far left and far right points, remove them from the
		   sorted sequence and store them in special members */
		left = list.front();

		list.erase( list.begin() );
		right = list.back();
		list.pop_back();
		
		/* Now put the remaining points in one of the two output sequences */
		for ( size_t i = 0 ; i < list.size() ; i++ )
		{
			int dir = direction( left, right, list[ i ] );
			if ( dir <0 )
				upper_partition_points.push_back( list[ i ] );
			else
				lower_partition_points.push_back( list[ i ] );
		}
	}
	void CGrahamScan::build_hull()
	{
		build_half_hull(lower_partition_points, lower_hull, 1 );
		build_half_hull(upper_partition_points, upper_hull, -1 );
	} 


	void CGrahamScan::build_half_hull(std::vector<std::pair<int,int>> input, std::vector<std::pair<int,int>> &output, int factor )
	{
		/* The hull will always start with the left point, and end with the right
		   point. Initialize input and output accordingly */
		input.push_back( right );
		output.push_back( left );

		while ( input.size() != 0 ) 
		{
			/* Repeatedly add the leftmost point to the null, then test to see 
			   if a convexity violation has occurred. If it has, fix things up
			   by removing the next-to-last point in the output sequence until 
			   convexity is restored. */
			output.push_back( input.front() );
			input.erase( input.begin() );
			while ( output.size()>= 3 ) 
			{
				size_t end = output.size() - 1;
				if ( factor * direction( output[ end - 2 ], output[ end ], output[ end - 1 ] ) <= 0 ) 
					output.erase( output.begin() + end - 1 );
				else
					break;
			}
		}
	} 


	std::pair<int,int> CGrahamScan::getPoint(int index)
	{
		if (index < (int) lower_hull.size())
			return lower_hull[index];
		
		int pos = index - lower_hull.size();
		    pos = upper_hull.size() - pos;
			pos--;
		return upper_hull[pos];		
	}
	// Graham Scan =====================================================	
	
	// Constructors
	CCluster::CCluster(CVolume *volume, char threshold, QImage &tf)
	{
		CCluster();
		this->volume = volume;

		int count = 0;

		// Insert voxel coordinates into the list
		for (int x = 0; x < volume->getWidth(); x++)
			for (int y = 0; y < volume->getHeight(); y++)
				for (int z = 0; z < volume->getDepth(); z++)
				{
					int intensity = volume->getVoxel(x, y, z);
					if (intensity < ((int) (unsigned char) threshold) || (tf.pixel(intensity, 0) <= qRgba(255,255,255,0)))
						continue;

					CVoxel<unsigned char> *vx = new CVoxel<unsigned char>((unsigned char) x, (unsigned char) y, (unsigned char) z);
					voxels.append( vx );
					count++;
				}
	}


	// Public Methods
	void CCluster::computeCenter()
	{
		std::list<CVoxel<unsigned char>> listofvoxels;

		float outpost = 0.0;
		// Compute center of the cluster, using simple mean
		for (int i = 0; i < this->voxels.size(); i++)
		{
			this->center.x += (float) voxels.at(i)->x;
			this->center.y += (float) voxels.at(i)->y;
			this->center.z += (float) voxels.at(i)->z;

			CVoxel<unsigned char> tmp(voxels.at(i)->x, voxels.at(i)->y, voxels.at(i)->z);

			outpost += volume->getVoxel(voxels.at(i)->x, voxels.at(i)->y, voxels.at(i)->z);
			listofvoxels.push_back(tmp);
		}
		outpost /= this->voxels.size();
		this->center.x /= this->voxels.size();
		this->center.y /= this->voxels.size();
		this->center.z /= this->voxels.size();

		//this->value = outpost;
		// ====================================================================
		// TODO: Compute local approximation of the scalar field at (x,y,z)
		// After inserting all voxels into the list and sorting them, we can then fetch the
		// 25 nearest neighbors to the centroid of the cluster
		xxx = this->center.x;
		yyy = this->center.y;
		zzz = this->center.z;
		
		listofvoxels.sort( compare_nocase );
		int s = (listofvoxels.size() < 25) ? listofvoxels.size() : 25;

		
		//list<CVoxel<unsigned char>>::iterator it = listofvoxels.begin();
		//for (int i = 0; i < s; i++)
		//{
		//	outpost += volume->getVoxel(it->x, it->y, it->z);
		//	it++;
		//}
		//listofvoxels.clear();
		//this->value = outpost / s;

		moreeq = 1;
		nun = neq = s;
		sys= new double*[neq]; //(double **)malloc((neq)*sizeof(double *));
		back = new double[nun]; //(double *)malloc((nun)*sizeof(double ));
		memset(back, 0, sizeof(double) * nun);
		for(int i=0;i<s;i++)
		{
			sys[i] = new double[nun+1];
			memset(sys[i], 0, sizeof(double) * (nun+1));
		}

		


// Descriptions are according to the paper, not the loop below
// a,b,c are the coordinates of j
// d,e,f are the coordinates of i
#define B(a, b, c, d, e, f) sqrt((a-d)*(a-d) + (b-e)*(b-e) + (c-f)*(c-f) + 0.000625)

		//int count = 0;

		CVoxel<unsigned char> *storage = new CVoxel<unsigned char>[s];
		std::list<CVoxel<unsigned char>>::iterator it = listofvoxels.begin();
		for (int i = 0; i < s; i++)
		{
			storage[i].x = it->x;
			storage[i].y = it->y;
			storage[i].z = it->z;
			it++;
		}
		listofvoxels.clear();

		for (int i = 0; i < s; i++)
		{
			sys[i][s] = (double) volume->getVoxel(storage[i].x, storage[i].y, storage[i].z);
			back[i] = 0.0;

			for (int j = 0; j < s; j++)
			{
				sys[i][j] += B(storage[i].x, storage[i].y, storage[i].z, 
					         storage[j].x, storage[j].y, storage[j].z);
			}
		}
		
		solution();

		float v = 0.0;
		for (int j = 0; j < s; j++)
		{
			v += back[j]*B(this->center.x, this->center.y, this->center.z, 
						 storage[j].x, storage[j].y, storage[j].z);
		}
		this->value = v;
		delete [] storage;
		delete [] back;
		for (int i = 0; i < s; i++)
			delete [] sys[i];
		delete [] sys;

		// ================================================================

		// Compute eigenbase
		float **symmat, **data;
	    float *evals, *interm;

		data    = matrix(this->voxels.size(), 4);
		symmat  = matrix(4, 4);  /* Duplicate of correlation (etc.) matrix */
		evals   = vector(4);     /* Storage alloc. for vector of eigenvalues */
		interm  = vector(4);    /* Storage alloc. for 'intermediate' vector */

		// Max Error
		int me = -999999;
		for (int i = 0; i < this->voxels.size(); i++)
		{
			data[i+1][1] = (float) voxels.at(i)->x;
			data[i+1][2] = (float) voxels.at(i)->y;
			data[i+1][3] = (float) voxels.at(i)->z;
			data[i+1][4] = (float) volume->getVoxel((int) voxels.at(i)->x, (int) voxels.at(i)->y, (int) voxels.at(i)->z);

			if (abs(data[i+1][4] - this->value) > me)
				me = abs(data[i+1][4] - this->value);
		}
		this->error = (unsigned char) me;

		covcol(data, this->voxels.size(), 4, symmat);

		tred2(symmat, 4, evals, interm);  /* Triangular decomposition */
		tqli(evals, interm, 4, symmat);   /* Reduction of sym. trid. matrix */

		// copy eigenbase to CCLuster eigenbase variable
		int eigenindex = 0;
		if (symmat[1][4] == 0.0 && symmat[1][3] == 0.0)
			eigenindex = 2;
		else
			eigenindex = 1;
		eigenBase[0] = symmat[eigenindex][4]; // X
		eigenBase[1] = symmat[eigenindex][3]; // Y
		eigenBase[2] = symmat[eigenindex][2]; // Z
		eigenBase[3] = symmat[eigenindex][1]; // Intensity

		free_vector(interm, 4);
		free_vector(evals,  4);
		free_matrix(symmat,  4, 4);
		free_matrix(data, this->voxels.size(), 4);
	}


	void CCluster::computeCenterMarchingClusters()
	{
		// Compute center of the cluster, using simple mean
		for (int i = 0; i < this->voxels.size(); i++)
		{
			this->center.x += (float) voxels.at(i)->x;
			this->center.y += (float) voxels.at(i)->y;
			this->center.z += (float) voxels.at(i)->z;
		}
		this->center.x /= this->voxels.size();
		this->center.y /= this->voxels.size();
		this->center.z /= this->voxels.size();

		// TODO: Compute local approximation of the scalar field at (x,y,z)
		this->value = 200;
		// ================================================================

		// Compute eigenbase
		float **symmat, **data;
	    float *evals, *interm;

		data    = matrix(this->voxels.size(), 4);
		symmat  = matrix(4, 4);  /* Duplicate of correlation (etc.) matrix */
		evals   = vector(4);     /* Storage alloc. for vector of eigenvalues */
		interm  = vector(4);    /* Storage alloc. for 'intermediate' vector */

		// Max Error
		int me = -999999;
		for (int i = 0; i < this->voxels.size(); i++)
		{
			data[i+1][1] = (float) voxels.at(i)->x/* - (int) this->center.x*/;
			data[i+1][2] = (float) voxels.at(i)->y/* - (int) this->center.y*/;
			data[i+1][3] = (float) voxels.at(i)->z/* - (int) this->center.z*/;
			data[i+1][4] = (float) volume->getVoxel((int) voxels.at(i)->x, (int) voxels.at(i)->y, (int) voxels.at(i)->z);

			if (abs(data[i+1][4] - this->value) > me)
				me = abs(data[i+1][4] - this->value);
		}
		this->error = (unsigned char) me;

		covcol(data, this->voxels.size(), 4, symmat);

		tred2(symmat, 4, evals, interm);  /* Triangular decomposition */
		tqli(evals, interm, 4, symmat);   /* Reduction of sym. trid. matrix */

		// copy eigenbase to CCLuster eigenbase variable
		int eigenindex = 0;
		if (symmat[1][4] == 0.0 && symmat[1][3] == 0.0)
			eigenindex = 2;
		else
			eigenindex = 1;
		eigenBase[0] = symmat[eigenindex][4]; // X
		eigenBase[1] = symmat[eigenindex][3]; // Y
		eigenBase[2] = symmat[eigenindex][2]; // Z
		eigenBase[3] = symmat[eigenindex][1]; // Intensity

		free_vector(interm, 4);
		free_vector(evals,  4);
		free_matrix(symmat,  4, 4);
		free_matrix(data, this->voxels.size(), 4);

		// Marching Cluster =========================================================================
		data    = matrix(this->voxels.size(), 3);
		symmat  = matrix(3, 3);  /* Duplicate of correlation (etc.) matrix */
		evals   = vector(3);     /* Storage alloc. for vector of eigenvalues */
		interm  = vector(3);    /* Storage alloc. for 'intermediate' vector */

		for (int i = 0; i < this->voxels.size(); i++)
		{
			data[i+1][1] = (float) voxels.at(i)->x - (int) this->center.x;
			data[i+1][2] = (float) voxels.at(i)->y - (int) this->center.y;
			data[i+1][3] = (float) voxels.at(i)->z - (int) this->center.z;
		}

		covcol(data, this->voxels.size(), 3, symmat);
		tred2(symmat, 3, evals, interm);  /* Triangular decomposition */
		tqli(evals, interm, 3, symmat);   /* Reduction of sym. trid. matrix */

		// Compute Convex Hull of projected voxels into eigenspace
		CGrahamScan gs;
//		float *datapoint = new float[voxels.size() * 3];

		// Compute projection of points into eigenspace
		for (int i = 0; i < this->voxels.size(); i++)
		{
			int x, y;
			x = data[i+1][1] * symmat[1][3] + data[i+1][2] * symmat[1][2] + data[i+1][3] * symmat[1][1];
			y = data[i+1][1] * symmat[2][3] + data[i+1][2] * symmat[2][2] + data[i+1][3] * symmat[2][1];
			
//			datapoint[i] = voxels.at(i/3)->x;
//			datapoint[i+1] = voxels.at(i/3)->y;
//			datapoint[i+2] = voxels.at(i/3)->z;
			gs.add(x, y);
		}
		gs.partition_points();
		gs.build_hull();

//		ch = new Chull3D(datapoint, voxels.size());
//		ch->compute();

		for (int i = 0; i < gs.size(); i++)\
		{
			std::pair<int, int> chp = gs.getPoint(i);

			int x, y, z;
			x = (int) chp.first * symmat[1][3] + chp.second * symmat[2][3];
			y = (int) chp.first * symmat[1][2] + chp.second * symmat[2][2];
			z = (int) chp.first * symmat[1][1] + chp.second * symmat[2][1];

			CVoxel<int> *p = new CVoxel<int>(x, y, z);
			convexhull.append( p );
		}

		free_vector(interm, 3);
		free_vector(evals,  3);
		free_matrix(symmat,  3, 3);
		free_matrix(data, this->voxels.size(), 3);
	}



	void CCluster::split(QList<CVoxel<unsigned char> *> *left, QList<CVoxel<unsigned char> *> *right)
	{
		for (int i = 0; i < voxels.size(); i++)
		{
			float tmpv[3];
			tmpv[0] = (voxels.at(i)->x - center.x) * eigenBase[0];
			tmpv[1] = (voxels.at(i)->y - center.y) * eigenBase[1];
			tmpv[2] = (voxels.at(i)->z - center.z) * eigenBase[2];

			if ((tmpv[0] + tmpv[1] + tmpv[2]) < 0)
				left->append(voxels.at(i));
			else
				right->append(voxels.at(i));
		}

		//while (voxels.size() > 0)
		//	delete voxels.takeFirst();
		voxels.clear();
	}



	// Private methods
	/**  Variance-covariance matrix: creation  *****************************/
	/* Create m * m covariance matrix from given n * m data matrix. */
	void CCluster::covcol(float **data, int n, int m, float **symmat)
	{
		int i, j, j1, j2;
	
		float *mean;
		mean = vector(4);

		/* Determine mean of column vectors of input data matrix */
		for (j = 1; j <= m; j++)
		{
			mean[j] = 0.0;
			for (i = 1; i <= n; i++)
			{
				mean[j] += data[i][j];
			}
			mean[j] /= (float)n;
		}

		/* Center the column vectors. */
		for (i = 1; i <= n; i++)
		{
			for (j = 1; j <= m; j++)
			{
				data[i][j] -= mean[j];
			}
		}

		/* Calculate the m * m covariance matrix. */
		for (j1 = 1; j1 <= m; j1++)
		{
			for (j2 = j1; j2 <= m; j2++)
			{
				symmat[j1][j2] = 0.0;
				for (i = 1; i <= n; i++)
				{
					symmat[j1][j2] += data[i][j1] * data[i][j2];
				}
				symmat[j2][j1] = symmat[j1][j2];
			}
		}
	}



	void CCluster::destroy()
	{
		if (voxels.size() > 0)
			while (voxels.size() > 0)
				delete voxels.takeFirst();
		voxels.clear();

		for (int i = 0; i < convexhull.size(); i++)
			delete convexhull.takeFirst();
	}
	float **CCluster::matrix(int n, int m)
	{
		int i;
		float **mat;

		/* Allocate pointers to rows. */
		mat = new float*[n];
		//mat = (float **) malloc((unsigned) (n)*sizeof(float*));
		if (mat == NULL)
			CMessageBox::getInstance()->writeMessage(QObject::tr("Not enough memory: trying to allocate memory to solve PCA."), VISc::mtError);
		mat -= 1;

		/* Allocate rows and set pointers to them. */
		for (i = 1; i <= n; i++)
		{
			//mat[i] = (float *) malloc((unsigned) (m)*sizeof(float));
			mat[i] = new float[m];
			if (mat[i] == NULL)
				CMessageBox::getInstance()->writeMessage(QObject::tr("Not enough memory: trying to allocate memory to solve PCA."), VISc::mtError);
			mat[i] -= 1;
		}

		 /* Return pointer to array of pointers to rows. */
		 return mat;
	}
	/* Allocates a float vector with range [1..n]. */
	float *CCluster::vector(int n)
	{
		float *v;

		//v = (float *) malloc ((unsigned) n*sizeof(float));
		v = new float[n];
		//if (!v) erhand("Allocation failure in vector().");
		if (v == NULL)
			CMessageBox::getInstance()->writeMessage(QObject::tr("Not enough memory: trying to allocate memory to solve PCA."), VISc::mtError);

		return v-1;
	}

	void CCluster::tqli(float d[], float e[], int n, float **z)
	{
		int m, l, iter, i, k;
		float s, r, p, g, f, dd, c, b;
		void erhand();

		for (i = 2; i <= n; i++)
			e[i-1] = e[i];

		e[n] = 0.0;
		for (l = 1; l <= n; l++)
		{
			iter = 0;
			do
			{
				for (m = l; m <= n-1; m++)
				{
				  dd = fabs(d[m]) + fabs(d[m+1]);
				  if (fabs(e[m]) + dd == dd) 
					  break;
				}

				if (m != l)
				{
					if (iter++ == 30) 
						CMessageBox::getInstance()->writeMessage(QObject::tr("No convergenve in TQLI."), VISc::mtError);
					g = (d[l+1] - d[l]) / (2.0 * e[l]);
					r = sqrt((g * g) + 1.0);
					g = d[m] - d[l] + e[l] / (g + SIGN(r, g));
					s = c = 1.0;
					p = 0.0;
					for (i = m-1; i >= l; i--)
					{
						f = s * e[i];
						b = c * e[i];
						
						if (fabs(f) >= fabs(g))
						{
							c = g / f;
							r = sqrt((c * c) + 1.0);
							e[i+1] = f * r;
							c *= (s = 1.0/r);
						}
						else
						{
							s = f / g;
							r = sqrt((s * s) + 1.0);
							e[i+1] = g * r;
							s *= (c = 1.0/r);
						}
						
						g = d[i+1] - p;
						r = (d[i] - g) * s + 2.0 * c * b;
						p = s * r;
						d[i+1] = g + p;
						g = c * r - b;
						
						for (k = 1; k <= n; k++)
						{
							f = z[k][i+1];
							z[k][i+1] = s * z[k][i] + c * f;
							z[k][i] = c * z[k][i] - s * f;
						}
					}
					d[l] = d[l] - p;
					e[l] = g;
					e[m] = 0.0;
				}
			}  while (m != l);
		}
	}
	
	/* Householder reduction of matrix a to tridiagonal form.
	   Algorithm: Martin et al., Num. Math. 11, 181-195, 1968.
	   Ref: Smith et al., Matrix Eigensystem Routines -- EISPACK Guide
			Springer-Verlag, 1976, pp. 489-494.
			W H Press et al., Numerical Recipes in C, Cambridge U P,
			1988, pp. 373-374.  */
	void CCluster::tred2(float **a, int n, float *d, float *e)
	{
		int l, k, j, i;
		float scale, hh, h, g, f;

		for (i = n; i >= 2; i--)
			{
			l = i - 1;
			h = scale = 0.0;
			if (l > 1)
			   {
			   for (k = 1; k <= l; k++)
				   scale += fabs(a[i][k]);
			   if (scale == 0.0)
				  e[i] = a[i][l];
			   else
				  {
				  for (k = 1; k <= l; k++)
					  {
					  a[i][k] /= scale;
					  h += a[i][k] * a[i][k];
					  }
				  f = a[i][l];
				  g = f>0 ? -sqrt(h) : sqrt(h);
				  e[i] = scale * g;
				  h -= f * g;
				  a[i][l] = f - g;
				  f = 0.0;
				  for (j = 1; j <= l; j++)
					  {
					  a[j][i] = a[i][j]/h;
					  g = 0.0;
					  for (k = 1; k <= j; k++)
						  g += a[j][k] * a[i][k];
					  for (k = j+1; k <= l; k++)
						  g += a[k][j] * a[i][k];
					  e[j] = g / h;
					  f += e[j] * a[i][j];
					  }
				  hh = f / (h + h);
				  for (j = 1; j <= l; j++)
					  {
					  f = a[i][j];
					  e[j] = g = e[j] - hh * f;
					  for (k = 1; k <= j; k++)
						  a[j][k] -= (f * e[k] + g * a[i][k]);
					  }
				 }
			}
			else
				e[i] = a[i][l];
			d[i] = h;
			}
		d[1] = 0.0;
		e[1] = 0.0;
		for (i = 1; i <= n; i++)
			{
			l = i - 1;
			if (d[i])
			   {
			   for (j = 1; j <= l; j++)
				   {
				   g = 0.0;
				   for (k = 1; k <= l; k++)
					   g += a[i][k] * a[k][j];
				   for (k = 1; k <= l; k++)
					   a[k][j] -= g * a[k][i];
				   }
			   }
			   d[i] = a[i][i];
			   a[i][i] = 1.0;
			   for (j = 1; j <= l; j++)
				   a[j][i] = a[i][j] = 0.0;
			}
	}



	/**  Deallocate vector storage  *********************************/
	/* Free a float vector allocated by vector(). */
	void CCluster::free_vector(float *v, int)
	{
		delete [] (v+1);
	}

	/**  Deallocate float matrix storage  ***************************/
	/* Free a float matrix allocated by matrix(). */
	void CCluster::free_matrix(float **mat, int n, int )
	{
		int i;

		for (i = n; i >= 1; i--)
			delete [] (mat[i]+1);

		delete [] (mat+1);
	}

	// =======================================================================
	double CCluster::evaluate(int g)
	{
	 int u;
	 double rt=0;
	 rt=sys[g][nun];
	 for(u=0;u<nun;u++)
	  {
	   if(g==u) continue;
	   rt-=sys[g][u]*back[u];
	  }
	 rt/=sys[g][g];
	 return rt;
	}

	int CCluster::consistent()
	{
	 int i,j,n=0;
	 for(i=0;i<neq;i++)
	  {
	   for(j=0;j<nun;j++)
		{
		 if(sys[i][j]!=0)
		 {
		  n++;
		  break;
		 }
		}
	  }
	  if(n==neq)
	   return 1;
	   return 0;
	}

	void CCluster::result()
	{
	int i;//,j;
	 if(!consistent())
	 {
//	  getch();
//	  clrscr();
//	  cout<<"\n\n Given system is inconsistent system...\n Solution can't be found\n";
//	  getch();
	  return;
	 }
	 else
	 {
	   for(i=nun-1;i>=0;i--)
		{
		 back[i]=evaluate(i);
		}
	   return;
	 }
	}

	void CCluster::solution()
	{
	  int i,j,k,r;
	  float c;
	  for(r=0;r<neq;r++)
	   {
		c=sys[r][r];

		  for(k=0;k<=nun;k++)
		   {
		sys[r][k]/=c;
		   }

		 for(i=r+1;i<neq;i++)
		  {
		   c=sys[i][r];
		for(j=0;j<=nun;j++)
		 {
		  sys[i][j]-=c*sys[r][j];
		 }
		  }

		for(i=0;i<neq;i++)
		  {
//		   cout<<"\n\n";
		 for(j=0;j<=nun;j++)
		  {
		   printf("%15.4f",sys[i][j]);
		  }
		  }
//		   cout<<"\n\n";
//	  getch();
	   }
	  result();
	}



}
