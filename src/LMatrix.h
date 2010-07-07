//-----------------------------------------------------------------------------
// This is the header file for the LMatrix class.  Specific class details can
// be found in the "LMatrix.cpp" file.  This is the same as the Matrix class
// except its elements are long doubles.
//-----------------------------------------------------------------------------

#ifndef LMATRIX_H
#define LMATRIX_H

#include <vector>

using namespace std;

class LMatrix {

	/**
	 * friend arithmetic operator multiplying a scalar k and matrix A
	 * (k * A)
	 */
  friend LMatrix operator*(const long double k, const LMatrix & A); 

	/**
	 * friend arithmetic operator multiplying a scalar k and matrix A
	 * (A * k)
	 */
  friend LMatrix operator*(const LMatrix & A, const long double k);  

	/**
	 * friend arithmetic operator raising a matrix A to the scalar k
	 * ( A^k )
	 */
  friend LMatrix operator^(const LMatrix & A, const int k);     

public:
	/**
	 * Default LMatrix Constructor constructs a 1x1 matrix of zeroes
	 */
	LMatrix();

	/**
	 * LMatrix Constructor constructs an nxm matrix of zeroes
	 */
	LMatrix(int n, int m);
    
	/**
	 * Data access function returning the number of rows
	 *
	 * @return rows
	 */
	int numRows() const;                   
    
	/**
	 * Data access function returning the number of rows
	 *
	 * @return cols
	 */
	int numCols() const;                    
    
	/**
	 * Data access function returning the element aij
	 */
	const long double & operator()(int i, int j) const; 

	/**
	 * Population Function
	 * sets value of element aij
	 *
	 * @param i row
	 * @param j column
	 * @param aij value of element
	 */
	void setElement(int i, int j, long double aij); 

	/**
	 * sets value of element A(i,j)
	 *
	 * @param i row
	 * @param j column
	 */
	long double & operator()(int i, int j);  // sets value of element A(i,j)

	/**
	 * adds a row at the end of the Matrix
	 *
	 * @param row is the row to add
	 */
	void addRow(vector<long double> row); 
	
	/**
	 * Prints the matrix
	 */
	void print() const;

	/**
	 * equals assignment operator for matrix objects
	 */
   const LMatrix & operator=(const LMatrix & rhs);
		
	/**
	 * addition assignment operator for matrix objects
	 */
   const LMatrix & operator+=(const LMatrix & rhs);

	/**
	 * subtraction assignment operator for matrix objects
	 */
   const LMatrix & operator-=(const LMatrix & rhs);

	/**
	 * multiplication assignment operator for matrix objects
	 */
   const LMatrix & operator*=(const LMatrix & rhs);

private:
	 /**
		* 2D vector containing matrix elements
		*/
   vector< vector<long double> > M; 
	 
	 /**
		 * number of rows
		 */
	 int rows;          

	 /**
		* number of columns
		*/
   int cols;                   

};

/**
 * addition operator for matrix objects A and B
 */
LMatrix operator+(const LMatrix & lhs, const LMatrix & rhs);  // A + B

/**
 * subtraction operator for matrix objects A and B
 */
LMatrix operator-(const LMatrix & lhs, const LMatrix & rhs);  // A - B

/**
 * multiplication operator for matrix objects A and B
 */
LMatrix operator*(const LMatrix & lhs, const LMatrix & rhs);  // A * B

/*
 * non-member function which creates an nxn identity matrix
 */
LMatrix identity(int n);  

#endif
