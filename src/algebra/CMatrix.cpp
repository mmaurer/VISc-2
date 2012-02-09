#include <math.h>
#include <iostream>
#include <exception>
using namespace std;

#include <QtGlobal>
#include <QtDebug>

#include "CMatrix.h"
#include "CPoint.h"
#include "CVector.h"

#define det3x3(a, b, c, d, e, f, g, h, i) (a*e*i + b*f*g+d*h*c - (g*e*c+h*f*a+d*b*i))

// Empty constructor
 CMatrix::CMatrix()
{
	this->setZero();
}

// Constructor from another matrix.
 CMatrix::CMatrix(const CMatrix &matrix) 
{
	for (int i=0; i<(this->height * this->width); i++)
		this->data[i] = matrix.data[i];

	//for (int i=0; i<4; i++)
	//	for (int j=0; j<4; j++)
	//		this->set(i, j, matrix.get(i, j));
}



// Copy constructor
 CMatrix::CMatrix(const CMatrix *matrix)
{
	for (int i=0; i < (this->height * this->width); i++)
		this->data[i] = matrix->data[i];

	//for (int i=0; i < CMatrix::width; i++)
	//	for (int j=0; j < CMatrix::height; j++)
	//		this->set(i, j, matrix->get(i, j));
}

/// Clears matrix
 void CMatrix::setZero()
{
	for (int i=0; i< (this->height * this->width); i++)
		this->data[i] = 0;
}



/// Sets as an identity matrix
 void CMatrix::setIdentity()
{
	this->setZero();

	this->set(0, 0, (double) 1.0);
	this->set(1, 1, (double) 1.0);
	this->set(2, 2, (double) 1.0);
	this->set(3, 3, (double) 1.0);
}



// Returns the inverse matrix of the current matrix.
// CMatrix CMatrix::invert() 
//{
//  // Copies the current matrix
//  CMatrix result(this);
//
//  // Indices
//  int i, j, k, row=0, col=0;
//
//  // General use variable
//  double value;
//
//  int* pivot    = new int[CMatrix::width];
//  int* indexRow = new int[CMatrix::width];
//  int* indexCol = new int[CMatrix::width];
//
//  for (i = 0; i < CMatrix::width; i++)
//    indexRow[i] = indexCol[i] = pivot[i] = 0;
//
//
//  for (i = 0; i < CMatrix::width; i++)
//  {
//    value = 0.0;
//
//    for (j = 0; j < CMatrix::width; j++)
//    {
//      if (pivot[j] != 1)
//      {
//        for (k = 0; k < CMatrix::width; k++)
//        {
//          if (pivot[k] == 0)
//          {
//            if (fabs(result.get(j, k)) >= value)
//            {
//              row   = j;
//              col   = k;
//              value = fabs(result.get(j, k));
//            }
//          }
//          else if (pivot[k] > 1)
//          {
//            delete[] pivot;
//            delete[] indexRow;
//            delete[] indexCol;
//            throw CException("Gauss-Jordan: Singular Matrix-1!", __FILE__, __LINE__);
//          }
//        }
//      }
//    }
//
//    ++(pivot[col]);
//
//    if (row != col)
//    {
//      for (j = 0; j < CMatrix::height; j++)
//      {
//				value = result.get(row, j);
//        result.set(row, j, result.get(col, j));
//        result.set(col, j, value);
//      }
//    }
//
//    indexRow[i] = row;
//    indexCol[i] = col;
//
//    if (result.get(col, col) == 0.0)
//    {
//      delete[] pivot;
//      delete[] indexRow;
//      delete[] indexCol;
//      throw CException("Gauss-Jordan: Singular Matrix-2!", __FILE__, /*__FUNC__,*/ __LINE__);
//      //throw exception();
//    }
//
//    value = 1.0 / result.get(col, col);
//    result.set(col, col, 1.0);
//
//    for (j = 0; j < CMatrix::height; j++)
//      result.set(col, j, result.get(col, j) * value);
//
//    for (j = 0; j < CMatrix::height; j++)
//      if (j != col)
//      {
//        value = result.get(j, col);
//        result.set(j, col, 0.0);
//
//        for (k = 0; k < CMatrix::height; k++)
//          result.set(j, k, result.get(j, k) - result.get(col, k) * value);
//      }
//  }
//
//  for (j = CMatrix::height - 1; j >= 0; j--)
//    if (indexRow[j] != indexCol[j])
//      for (k = 0; k < CMatrix::height; k++)
//      {
//        value = result(k, indexRow[j]);
//        result.set(k, indexRow[j], result.get(k, indexCol[j]));
//        result.set(k, indexCol[j], value);
//      }
//
//  delete[] pivot;
//  delete[] indexRow;
//  delete[] indexCol;
//
//  // Retorna a matriz inversa
//  return result;
//}
// Returns the inverse matrix of the current matrix by Adjoint Method
CMatrix CMatrix::invert()
{
	CMatrix tmp;
	double det = determinant();

	if (det == 0.0)
	{
		tmp.setIdentity();
		return tmp;
	}

	double inv = 1.0 / det;

	CMatrix adjoint = this->adjoint();
	tmp = adjoint * inv;

	return tmp;
}
// Returns the adjoint of the matrix
CMatrix CMatrix::adjoint()
{
	CMatrix result;

    double a1, a2, a3, a4, b1, b2, b3, b4;
    double c1, c2, c3, c4, d1, d2, d3, d4;

    /* assign to individual variable names to aid  */
    /* selecting correct values  */
	a1 = get(0,0); b1 = get(0,1); 
	c1 = get(0,2); d1 = get(0,3);

	a2 = get(1,0); b2 = get(1,1); 
	c2 = get(1,2); d2 = get(1,3);

	a3 = get(2,0); b3 = get(2,1);
	c3 = get(2,2); d3 = get(2,3);

	a4 = get(3,0); b4 = get(3,1); 
	c4 = get(3,2); d4 = get(3,3);


    /* row column labeling reversed since we transpose rows & columns */

    result.set(0,0,   det3x3( b2, b3, b4, c2, c3, c4, d2, d3, d4));
    result.set(1,0, - det3x3( a2, a3, a4, c2, c3, c4, d2, d3, d4));
    result.set(2,0,   det3x3( a2, a3, a4, b2, b3, b4, d2, d3, d4));
    result.set(3,0, - det3x3( a2, a3, a4, b2, b3, b4, c2, c3, c4));
        
    result.set(0,1, - det3x3( b1, b3, b4, c1, c3, c4, d1, d3, d4));
    result.set(1,1,   det3x3( a1, a3, a4, c1, c3, c4, d1, d3, d4));
    result.set(2,1, - det3x3( a1, a3, a4, b1, b3, b4, d1, d3, d4));
    result.set(3,1,   det3x3( a1, a3, a4, b1, b3, b4, c1, c3, c4));
        
    result.set(0,2,   det3x3( b1, b2, b4, c1, c2, c4, d1, d2, d4));
    result.set(1,2, - det3x3( a1, a2, a4, c1, c2, c4, d1, d2, d4));
    result.set(2,2,   det3x3( a1, a2, a4, b1, b2, b4, d1, d2, d4));
    result.set(3,2, - det3x3( a1, a2, a4, b1, b2, b4, c1, c2, c4));
        
    result.set(0,3, - det3x3( b1, b2, b3, c1, c2, c3, d1, d2, d3));
    result.set(1,3,   det3x3( a1, a2, a3, c1, c2, c3, d1, d2, d3));
    result.set(2,3, - det3x3( a1, a2, a3, b1, b2, b3, d1, d2, d3));
    result.set(3,3,   det3x3( a1, a2, a3, b1, b2, b3, c1, c2, c3));

	return result;
}
// Overloading =
/*CMatrix CMatrix::operator= (CMatrix &matrix)
{
	

	for (int i=0; i < CMatrix::width; i++)
		for (int j=0; j < CMatrix::height; j++)
			this->set(i,j, matrix.get(i ,j));
	this->det = matrix.determinant();

	return *this;
} */
// Overloading * by Matrix
void CMatrix::transpose()
{
	CMatrix newMatrix;

	for(int i = 0; i < CMatrix::width; i++)
		for (int j = 0; j < CMatrix::height; j++)
			newMatrix.set(j, i, this->get(i, j));

	for (int i = 0; i < CMatrix::width * CMatrix::height; i++)
		this->data[i] = newMatrix.data[i];
}
 CMatrix CMatrix::operator* (CMatrix &matrix) 
{
	CMatrix newMatrix;

	for (int i=0; i < this->height; i++)
		for (int j=0; j < this->height; j++)
			for (int k=0; k < this->height; k++)
				newMatrix.set(i, j, newMatrix.get(i,j) + this->get(k, j) * matrix.get(i, k));

	return newMatrix;
}


// Overloading ==
 bool CMatrix::operator== (CMatrix &matrix) 
{
	for (int i=0; i < this->width; i++)
		for (int j=0; j < this->height; j++)
			if (this->get(i ,j) != matrix.get(i ,j))
				return false;
	return true;
}


// Overloading + by Matrix
 CMatrix CMatrix::operator+ (CMatrix &matrix) 
{
	CMatrix newMatrix;

	for (int i=0; i < this->width; i++)
		for (int j=0; j < this->height; j++)
			newMatrix.set(i, j, this->get(i,j) + matrix.get(i,j));

 	return newMatrix;
}



// Overloading - by Matrix
 CMatrix CMatrix::operator- (CMatrix &matrix)
{
	CMatrix newMatrix;

	for (int i=0; i < this->width; i++)
		for (int j=0; j < this->height; j++)
			newMatrix.set(i, j, this->get(i,j) - matrix.get(i,j));

  return newMatrix;  
}



// Overloading * by Scalar
 CMatrix CMatrix::operator* (double scalar)
{
	CMatrix newMatrix;

	for (int i=0; i < this->width; i++)
		for (int j=0; j < this->height; j++)
			newMatrix.set(i, j, this->get(i,j) * scalar);

	return newMatrix;  
}



 CMatrix CMatrix::operator/ (double scalar)
 {
	CMatrix newMatrix;
	
    if (scalar == 0.0)
	{
		for (int i=0; i < this->width; i++)
			for (int j=0; j < this->height; j++)
				newMatrix.set(i, j, this->get(i,j));
	}
	else
	{
		for (int i=0; i < this->width; i++)
			for (int j=0; j < this->height; j++)
				newMatrix.set(i, j, this->get(i,j) / scalar);
	}
	
	return newMatrix;
 }
// Overloading + by Scalar
 CMatrix CMatrix::operator+ (double scalar) 
{
	CMatrix newMatrix;

	for (int i=0; i < this->width; i++)
		for (int j=0; j < this->height; j++)
			newMatrix.set(i, j, this->get(i,j) + scalar);

  return newMatrix;    
}



// Overloading - by Scalar
 CMatrix CMatrix::operator- (double scalar)
{
	CMatrix newMatrix;

	for (int i=0; i < this->width; i++)
		for (int j=0; j < this->height; j++)
			newMatrix.set(i, j, this->get(i,j) - scalar);

  return newMatrix;    
}

// Overloading ()
 double CMatrix::operator() (int column, int line)
{
	if (column > this->width)
		cerr << "Range of matrix index element out of range: (" << column << "," << line << ") " << __FILE__ << " " << __LINE__ << endl;
	if (line > this->height)
		cerr << "Range of matrix index element out of range: (" << column << "," << line << ") " << __FILE__ << " " << __LINE__ << endl;

	return this->get(column, line);
}

// Overloading * by Point
 CPoint CMatrix::operator* (CPoint &point) 
{
	CPoint newPoint;

	for (int i=0; i < this->width; i++)
		for (int k=0; k < this->height; k++)
			newPoint.set(VISc::MathUtils::IntToComponent(i), newPoint.get(VISc::MathUtils::IntToComponent(i)) + this->get(k, i) * point.get(VISc::MathUtils::IntToComponent(k)));

	return newPoint;
}

 // Overloading * by Vector
CVector CMatrix::operator* (CVector &vector)
{
	CVector newVector;

	newVector.setX( this->get(0, 0) * vector.getX() + this->get(1, 0) * vector.getY() + this->get(2, 0) * vector.getZ() + this->get(3, 0) );
	newVector.setY( this->get(0, 1) * vector.getX() + this->get(1, 1) * vector.getY() + this->get(2, 1) * vector.getZ() + this->get(3, 1) );
	newVector.setZ( this->get(0, 2) * vector.getX() + this->get(1, 2) * vector.getY() + this->get(2, 2) * vector.getZ() + this->get(3, 2) );

	return newVector;
}
/// Sets position (column, line) with value
 void CMatrix::set(int column, int line, double value) 
{
	if (column > this->width)
		cerr << "Range of matrix index element out of range: ("<< column << "," << line << ") " << __FILE__ << " " << __LINE__ << endl;
	if (line > this->height)
		cerr << "Range of matrix index element out of range: (" << column << "," << line << ") " << __FILE__ << " " << __LINE__ << endl;

	int position = (line * this->width) + column;

	this->data[position] = value;
}

///
 void CMatrix::setGlMatrix(double *m)
{
	this->set(0, 0, m[0]);
	this->set(0, 1, m[1]);
	this->set(0, 2, m[2]);
	this->set(0, 3, m[3]);
	this->set(1, 0, m[4]);
	this->set(1, 1, m[5]);
	this->set(1, 2, m[6]);
	this->set(1, 3, m[7]);
	this->set(2, 0, m[8]);
	this->set(2, 1, m[9]);
	this->set(2, 2, m[10]);
	this->set(2, 3, m[11]);
	this->set(3, 0, m[12]);
	this->set(3, 1, m[13]);
	this->set(3, 2, m[14]);
	this->set(3, 3, m[15]);
}
///
void CMatrix::set(float m[16])
 {
	 for (int i = 0; i < 16; i++)
		 data[i] = m[i];
 }
/// Returns the value in position (column, line)
double CMatrix::get (int column, int line)
{
	if (column > this->width)
		cerr << "Range of matrix index element out of range: (" << column << "," << line << ") " << __FILE__ << " " << __LINE__ << endl;
	if (line > this->height)
		cerr << "Range of matrix index element out of range: (" << column << "," << line << ") " << __FILE__ << " " << __LINE__ << endl;


	int position = (line * this->width) + column;

	return this->data[position];
}
 ///
void CMatrix::getEulerAngles(float &x, float &y, float &z)
{
	//%%%%%%%%%%%%%
	// NOT WORKING
	//%%%%%%%%%%%%%

	double angle_x, angle_y, angle_z;		// temp angles
	double cy, tx, ty, sy;
/*
	angle_y = -asin(m[2]);		// calculate y axis angle
	cy		= cos(angle_y);
*/

	sy	= -data[2];
	cy	= sqrt(1 - std::pow(sy, 2.0));
	angle_y	= atan2(sy, cy);


	if (VISc::MathUtils::EPSILON < fabs(cy))		// no gimbal lock?
	{
		tx	= data[10] / cy;		// get x axis angle
		ty	= data[6] / cy;

		angle_x	= atan2(ty, tx);

		tx	= data[0] / cy;		// get z axis angle
		ty	= data[1] / cy;

		angle_z	= atan2(ty, tx);
	}
	else		// gimbal lock
	{
		angle_x	= 0.0f;			// set x axis angle to 0

		tx	= data[9];				// get z axis angle
		ty	= data[5];

		angle_z	= atan2(ty, tx);
	}

	x	= (float)VISc::MathUtils::RadiansToDegrees(angle_x);		// convert all to deg
	y	= (float)VISc::MathUtils::RadiansToDegrees(angle_y);
	z	= (float)VISc::MathUtils::RadiansToDegrees(angle_z);
/*
	LIMIT_RANGE(-180.0f, x, 180.0f);	// and limit them
	LIMIT_RANGE(-180.0f, y, 180.0f);
	LIMIT_RANGE(-180.0f, z, 180.0f);
*/
	return;
}	// end void GetEulerAngles(..)


 ///
void CMatrix::getGlMatrix(double *m)
 {
	 m[0]  = this->get(0, 0);
	 m[1]  = this->get(0, 1);
	 m[2]  = this->get(0, 2);
	 m[3]  = this->get(0, 3);
	 m[4]  = this->get(1, 0);
	 m[5]  = this->get(1, 1);
	 m[6]  = this->get(1, 2);
	 m[7]  = this->get(1, 3);
	 m[8]  = this->get(2, 0);
	 m[9]  = this->get(2, 1);
	 m[10] = this->get(2, 2);
	 m[11] = this->get(2, 3);
	 m[12] = this->get(3, 0);
	 m[13] = this->get(3, 1);
	 m[14] = this->get(3, 2);
	 m[15] = this->get(3, 3);
 }
/// Check emptyness of matrix
bool CMatrix::isEmpty()
{
	for (int i=0; i < (this->width * this->height); i++)
		if (this->data[i] != (double) 0.0)
			return false;

	return true;
}
/// Check if matrix is identity
bool CMatrix::isIdentity()
{
	for (int i=0; i < this->width; i++)
		for (int j=0; j < this->height; j++)
		{
			if (i == j)
			{
				if (this->get(i,j) != (double) 1.0)
					return false;
			}
			else
			{
				if (this->get(i,j) != (double) 0.0)
					return false;
			}
		}
	return true;
}

/// Copy matrix to a brand new one
CMatrix CMatrix::copy()
{
	CMatrix newMatrix;

	for (int i=0; i < this->width; i++)
		for (int j=0; j < this->height; j++)
			newMatrix.set(i, j, this->get(i, j));

	return newMatrix;
}

///
void CMatrix::print()
{
	qDebug() << endl;
	qDebug() << this->get(0,0) << "\t" << this->get(1,0) << "\t" << this->get(2,0) << "\t" << this->get(3,0) << endl;
	qDebug() << this->get(0,1) << "\t" << this->get(1,1) << "\t" << this->get(2,1) << "\t" << this->get(3,1) << endl;
	qDebug() << this->get(0,2) << "\t" << this->get(1,2) << "\t" << this->get(2,2) << "\t" << this->get(3,2) << endl;
	qDebug() << this->get(0,3) << "\t" << this->get(1,3) << "\t" << this->get(2,3) << "\t" << this->get(3,3) << endl;
}


double CMatrix::determinant()
{
    double a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;

    /* assign to individual variable names to aid selecting */
	/*  correct elements */

	a1 = get(0,0); b1 = get(0,1); 
	c1 = get(0,2); d1 = get(0,3);

	a2 = get(1,0); b2 = get(1,1); 
	c2 = get(1,2); d2 = get(1,3);

	a3 = get(2,0); b3 = get(2,1); 
	c3 = get(2,2); d3 = get(2,3);

	a4 = get(3,0); b4 = get(3,1); 
	c4 = get(3,2); d4 = get(3,3);

    double ans = a1 * det3x3( b2, b3, b4, c2, c3, c4, d2, d3, d4)
			   - b1 * det3x3( a2, a3, a4, c2, c3, c4, d2, d3, d4)
			   + c1 * det3x3( a2, a3, a4, b2, b3, b4, d2, d3, d4)
			   - d1 * det3x3( a2, a3, a4, b2, b3, b4, c2, c3, c4);
    return ans;
}
