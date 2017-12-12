//-----------------------------------------------------------------------------
// This is the header file for the LMatrix class.  Specific class details can
// be found in the "l_matrix.cc" file.  This is the same as the Matrix class
// except its elements are long doubles.
//-----------------------------------------------------------------------------

#ifndef CYCLUS_SRC_L_MATRIX_H_
#define CYCLUS_SRC_L_MATRIX_H_

#include <vector>

namespace cyclus {

class LMatrix {
  // friend arithmetic operators involving a scalar k and matrix A
  friend LMatrix operator*(const long double k, const LMatrix& A);   // k * A
  friend LMatrix operator*(const LMatrix& A, const long double k);   // A * k
  friend LMatrix operator^(const LMatrix& A, const int k);      // A^k

 public:
  // constructors
  LMatrix();              // constructs a 1x1 matrix of zeroes
  LMatrix(int n, int m);  // constructs an nxm matrix of zeroes

  // member access functions
  int NumRows() const;                    // returns number of rows
  int NumCols() const;                    // returns number of columns
  const long double& operator()(int i, int j) const;   // returns the element aij

  // population functions
  void SetElement(int i, int j, long double aij);  // sets value of element aij
  long double& operator()(int i, int j);   // sets value of element A(i,j)
  void AddRow(std::vector<long double>
              row);  // adds a row at the end of the Matrix

  // other member functions
  void Print() const;  // prints the matrix

  // assignment operators for matrix objects
  const LMatrix& operator=(const LMatrix& rhs);
  const LMatrix& operator+=(const LMatrix& rhs);
  const LMatrix& operator-=(const LMatrix& rhs);
  const LMatrix& operator*=(const LMatrix& rhs);

 private:
  std::vector< std::vector<long double> >
      M_;  // 2D vector containing matrix elements
  int rows_;                    // number of rows
  int cols_;                    // number of columns
};

// arithmetic operators for matrix objects A and B
LMatrix operator+(const LMatrix& lhs, const LMatrix& rhs);    // A + B
LMatrix operator-(const LMatrix& lhs, const LMatrix& rhs);    // A - B
LMatrix operator*(const LMatrix& lhs, const LMatrix& rhs);    // A * B

// non-member functions
LMatrix identity(int n);  // creates an nxn identity matrix

}  // namespace cyclus

#endif  // CYCLUS_SRC_L_MATRIX_H_
