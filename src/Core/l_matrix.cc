//-----------------------------------------------------------------------------
// A LMatrix object contains n rows and m columns.  When a LMatrix object is
// created, it is initialized with zeros in all of the elements by default.
// To change the value of the element aij at row i and column j, use the
// SetElement(int i, int j, long double aij) function.  The only difference
// between LMatrix and Matrix is that LMatrix elements are long doubles instead
// of doubles.
//
// The function NumRows() returns the number of rows n in the LMatrix object,
// and the function NumCols() returns the number of columns m in the LMatrix
// object.  To access the element in the ith row and jth column of a LMatrix A,
// use the syntax A(i,j).  To print the matrix, use the Print() function.
//
// Mathematical functions that can be performed on two LMatrix objects include:
//
//   * matrix assignment:        A = B
//   * matrix addition:          A + B
//   * matrix subtraction:       A - B
//   * matrix multiplication:    A * B
//
// Mathematical functions that can be performed on a LMatrix object A and a
// double k include:
//
//   * powers of a matrix:       A ^ k
//   * scalar multiplication:    k * A or A * k
//
// The non-member function identity(int n) creates and returns an nxn identity
// matrix.
//
// Note: when referring to the elements of a LMatrix object, the indices for the
// rows and columns start from 1.
//-----------------------------------------------------------------------------

#include "l_matrix.h"

#include <iostream>
#include <iomanip>

namespace cyclus {

// constructs a 1x1 matrix of zeroes
LMatrix::LMatrix() {
  rows_ = 1;                   // sets number of rows
  cols_ = 1;                   // sets number of columns
  std::vector<long double> element(1);  // creates a single element
  M_.push_back(element);       // adds element to the matrix
}

// constructs an nxm matrix of zeroes
LMatrix::LMatrix(int n, int m) {
  rows_ = n;                     // sets number of rows
  cols_ = m;                     // sets number of columns
  std::vector<long double> row(m);   // creates a row with m elements
  M_.assign(n, row);              // adds n rows_ to the matrix
}

// returns the number of rows n in the matrix
int LMatrix::NumRows() const {
  return rows_;
}

// returns the number of columns m in the matrix
int LMatrix::NumCols() const {
  return cols_;
}

// overloads the () operator so that A(i,j) will return a reference to
// the element aij
const long double& LMatrix::operator()(int i, int j) const {
  return M_[i - 1][j - 1];
}

// sets the value for the element aij at row i and column j
void LMatrix::SetElement(int i, int j, long double aij) {
  M_[i - 1][j - 1] = aij;
}

// overloads the () operator so that A(i,j) will write the element aij
long double& LMatrix::operator()(int i, int j) {
  return M_[i - 1][j - 1];
}

// adds a row at the end of the Matrix if it contains the same number of
// elements as the number of columns in the Matrix
void LMatrix::AddRow(std::vector<long double> row) {
  int size = row.size();
  if (size == cols_) {
    M_.push_back(row);
    ++rows_;  // increase the number of rows
  }
}

// prints the matrix to standard output
void LMatrix::Print() const {
  std::cout.setf(std::ios::showpoint);
  std::cout.setf(std::ios::scientific);

  // sets elements to display 6 decimal places
  std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(10);

  // prints single element if M is a 1x1 matrix
  if (M_.capacity() == 1 && M_[0].capacity() == 1) {
    std::cout << "[ " << M_[0][0] << " ]" << std::endl;
  } else {
    // loops through the rows of the matrix
    for (int i = 0; i < rows_; i++) {
      // prints all of the elements in the ith row of the matrix
      std::cout << "[";
      for (int j = 0; j < cols_ - 1; j++) {
        std::cout << "  " << std::setw(9) << M_[i][j]  << "  ";
      }
      std::cout << std::setw(9) << M_[i][cols_ - 1] << "  ]" << std::endl;
    }
  }
}

// overloads the assignment operator "A = B" for matrix objects
const LMatrix& LMatrix::operator=(const LMatrix& rhs) {
  if (this == &rhs) {
    return *this;     // returns A if it is already the same matrix as B
  } else {
    rows_ = rhs.rows_;  // resets the number of rows to match B
    cols_ = rhs.cols_;  // resets the number of columns to match B

    // rebuilds A with the dimensions of B
    std::vector<long double> row(cols_);   // creates a row with m elements
    M_.assign(rows_, row);

    // copies all of the elements from B into A
    for (int i = 0; i < rows_; i++) {
      for (int j = 0; j < cols_; j++) {
        M_[i][j] = rhs.M_[i][j];
      }
    }

    return *this;  // returns the new matrix "A = B"
  }
}

// overloads the assignment operator "A = A + B" for matrix objects
// Note: if the matrix dimensions do not match, then A is returned unchanged
const LMatrix& LMatrix::operator+=(const LMatrix& rhs) {
  if (this->rows_ == rhs.rows_ && this->cols_ == rhs.cols_) {
    // performs matrix addition and stores the result in A
    for (int i = 0; i < rows_; i++) {
      for (int j = 0; j < cols_; j++) {
        M_[i][j] += rhs.M_[i][j];
      }
    }
  }

  return *this;  // returns the new matrix "A = A + B"
}

// overloads the assignment operator "A = A - B" for matrix objects
// Note: if the matrix dimensions do not match, then A is returned unchanged
const LMatrix& LMatrix::operator-=(const LMatrix& rhs) {
  if (this->rows_ == rhs.rows_ && this->cols_ == rhs.cols_) {
    // performs matrix subtraction and stores the result in A
    for (int i = 0; i < rows_; i++) {
      for (int j = 0; j < cols_; j++) {
        M_[i][j] -= rhs.M_[i][j];
      }
    }
  }

  return *this;  // returns the new matrix "A = A - B"
}

// overloads the assignment operator "A = A * B" for matrix objects
//
// Note: This function will perform matrix multiplication only if the number
// of columns in A is equal to the number of rows in B.  If the matrices
// cannot be multipled due to having incorrect dimensions for matrix
// multiplication to be defined, then A will be returned unchanged
const LMatrix& LMatrix::operator*=(const LMatrix& rhs) {
  long double aij;  // stores temporary matrix element for row i and column j

  // creates a new matrix called temp with the number of rows of A and the
  // number of columns of B
  LMatrix temp(this->rows_, rhs.cols_);

  // performs matrix multiplication if the number of columns of A equals the
  // number of rows of B
  if (this->cols_ == rhs.rows_) {
    // multiplies row vector i of A by the column vector j of B and stores
    // the result in the new matrix temp as element aij
    for (int i = 0; i < rows_; i++) {
      for (int j = 0; j < rhs.cols_; j++) {
        aij = 0;  // resets the sum for the next element

        // sums the product of the row vector from A and the column vector
        // from B using the formula aij = ai1*b1j + ai2*b2j + ...
        for (int k = 0; k < rows_; k++) {
          aij = aij + M_[i][k] * rhs.M_[k][j];
        }

        temp.M_[i][j] = aij;  // copies element aij into temp
      }
    }

    *this = temp;  // sets the new matrix "A = A * B"
  }

  return *this;  // returns the new matrix "A = A * B"
}

// overloads the arithmetic operator "A + B" for matrix objects

LMatrix operator+(const LMatrix& lhs, const LMatrix& rhs) {
  LMatrix ans(lhs);
  ans += rhs;
  return ans;  // returns the resulting matrix A + B
}

// overloads the arithmetic operator "A - B" for matrix objects

LMatrix operator-(const LMatrix& lhs, const LMatrix& rhs) {
  LMatrix ans(lhs);
  ans -= rhs;
  return ans;  // returns the resulting matrix A - B
}

// overloads the arithmetic operator "A * B" for matrix objects

LMatrix operator*(const LMatrix& lhs, const LMatrix& rhs) {
  LMatrix ans(lhs);
  ans *= rhs;
  return ans;  // returns the resulting matrix A * B
}

// friend of the Matrix class that performs scalar multiplication k * A

LMatrix operator*(const long double k, const LMatrix& A) {
  LMatrix ans(A);  // copies A into a new matrix called ans
  long double aij;

  // multiplies every element in the matrix A by the scalar k
  for (int i = 0; i < A.rows_; i++) {
    for (int j = 0; j < A.cols_; j++) {
      aij = A(i + 1, j + 1) * k;
      ans.M_[i][j] = aij;
    }
  }

  return ans;  // returns the resulting matrix k * A
}

// friend of the Matrix class that performs scalar multiplication A * k

LMatrix operator*(const LMatrix& A, const long double k) {
  LMatrix ans(A);  // copies A into a new matrix called ans
  long double aij;

  // multiplies every element in the matrix A by the scalar k
  for (int i = 0; i < A.rows_; i++) {
    for (int j = 0; j < A.cols_; j++) {
      aij = A(i + 1, j + 1) * k;
      ans.M_[i][j] = aij;
    }
  }

  return ans;  // returns the resulting matrix A * k
}

// friend of the Matrix class that calculates powers of a square matrix A ^ k
// Note: if the matrix is not square, then A is returned unchanged

LMatrix operator^(const LMatrix& A, const int k) {
  LMatrix ans(A);  // copies A into a new matrix called ans

  // multiplies A by itself k times if the matrix is square
  if (A.cols_ == A.rows_) {
    for (int i = 0; i < k - 1; i++) {
      ans *= A;
    }
  }

  return ans;  // returns the resulting matrix A ^ k
}

// creates and returns an nxn identity matrix I

LMatrix identity(int n) {
  LMatrix I(n, n);

  for (int i = 0; i < n ; i++) {
    I.SetElement(i + 1, i + 1, 1);
  }

  return I;  // returns the nxn identity matrix
}
} // namespace cyclus
