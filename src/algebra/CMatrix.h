#ifndef CMATRIX_H
#define CMATRIX_H

#include "MathUtils.h"
#include "CPoint.h"

#if defined(WIN32) && defined(_DEBUG)
   #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
   #define new DEBUG_NEW
#endif

/// A matrix of doubles
/** Although a Template Matrix could be use instead of this rigid representation,
the Double Matrix is preferred because of double type usage: other types of matrixes
are not used in VISc platform. To avoid future conversion problems or confusions,
the only matrix type available is the one that use doubles, sacrificing flexibility for simplicity.
*/
	class CMatrix {
		// Public stuff
		public:
			// Empty Constructor. Assumes the matrix will be 4x4
			CMatrix();  // Default constructor
		  
			// Constructors
			CMatrix(const CMatrix &matrix); // Copy constructor
			CMatrix(const CMatrix *matrix);

			// Copy current matrix to a brand new one
			CMatrix copy();
		  
			// Returns the inverse matrix of the current matrix.
			CMatrix invert(); /// Returns the inverse matrix of the current matrix.
			//CMatrix invertAdjoint(); /// Returns the inverse matrix of the current matrix by Adjoint Method
		  
			//// Transpose Matrix
			void transpose (); /// Transpose matrix

			// Overloading (x, y)
			double operator()(int column, int line);

			// Overloading ==
			bool operator== (CMatrix &matrix);

			// Overloading =
		 // CMatrix operator= (CMatrix &matrix);
		  
			// Overloading * by Matrix
			CMatrix operator* (CMatrix &matrix); 
		  
			// Overloading + by Matrix
			CMatrix operator+ (CMatrix &matrix);
		    
			// Overloading - by Matrix
			CMatrix operator- (CMatrix &matrix);
		    
			// Overloading * by Scalar
			CMatrix operator* (double scalar);

			// Overloading / by Scalar
			CMatrix operator/ (double scalar);
		    
			// Overloading + by Scalar
			CMatrix operator+ (double scalar);

			// Overloading - by Scalar
			CMatrix operator- (double scalar);
		      
			// Overloading * by Point
			CPoint operator* (CPoint &point);
		    
			// Overloading * by Vector
			CVector operator* (CVector &vector);

			//! 
			// @param column 
			// @param line 
			// @param value 
			void set(int column, int line, double value);
			void set(float m[16]);
			void setGlMatrix(double *m);

			// 
			// @param column 
			// @param line 
			double get(int column, int line);
			void getEulerAngles(float &x, float &y, float &z);
			void getGlMatrix(double *m);

			/// Clears matrix (sets all elements to zero)
			void setZero();

			/// Set the matrix to identity matrix
			void setIdentity();

			/// Check emptyness of matrix
			bool isEmpty();

			/// Check if matrix is identity
			bool isIdentity();

			void print();

			double determinant();

			CMatrix adjoint();

		private:
			// Fields
			 static const int height = 4;
			 static const int width = 4;
			 double data[height * width]; /// Data

			 // Allows optimized operators implementation
			 //friend class CMatrix;
	 
	};
#endif //CMATRIX_H
