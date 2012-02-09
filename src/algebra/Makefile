all: CMatrix CVector CPoint Algebra
	ar rc libalgebra.a *.o

CMatrix: CMatrix.cpp
	g++ CMatrix.cpp -c -o CMatrix.o -O3 -Wall

CVector: CVector.cpp
	g++ CVector.cpp -c -o CVector.o -O3 -Wall

CPoint: CPoint.cpp
	g++ CPoint.cpp -c -o CPoint.o -O3 -Wall

Algebra: Algebra.cpp
	g++ Algebra.cpp -c -o Algebra.o -O3 -Wall

clean:
	-rm *.o libalgebra.a
