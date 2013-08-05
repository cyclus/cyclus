// UniformTaylor.cpp
// Implements the UniformTaylor class
#include "UniformTaylor.h"

#include "error.h"

#include <cmath>
#include <string>

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Vector UniformTaylor::MatrixExpSolver(const Matrix & A,
    const Vector & x_o, const double t)
{
  int n = A.NumRows();
  
  // checks if the dimensions of A and x_o are compatible for matrix-vector
  // computations
  if ( x_o.NumRows() != n ) {
    std::string error = "Error: Matrix-Vector dimensions are not compatible.";
    throw ValueError(error);
  }

  // step 1 of algorithm: calculates the largest diagonal element (alpha)
  double alpha = MaxAbsDiag(A);
  
  // step 2 of algorithm: creates the matrix B = A + alpha * I
  Matrix B = identity(n);
  B = alpha * B;
  B += A;
  
  // steps 3-7 of algorithm: computes the solution Vector x_t
  double tol = 1e-3;
  Vector x_t = x_o;

  x_t = GetSolutionVector(B, x_o, alpha, t, tol);

  return x_t;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double UniformTaylor::MaxAbsDiag(const Matrix & A) {
  int n = A.NumRows();       // stores the order of the matrix A    
  double a_ii = A(1,1);      // begins with the first diagonal element

  // Initializes the maximum diagonal element to the absolute value of the
  // first diagonal element a_ii
  a_ii = fabs(a_ii);         
  double max_a_ii = a_ii;  
  
  // Searches the remaining diagonal elements for the largest absolute value 
  for ( int i = 2; i <= n; ++i ) {
    a_ii = A(i,i);
    a_ii = fabs(a_ii); 
      
    if ( a_ii > max_a_ii ) { 
      max_a_ii = a_ii;
    }
  }
  
  return max_a_ii; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Vector UniformTaylor::GetSolutionVector(const Matrix & B,
    const Vector & x_o, double alpha, double t, double tol)
{
  // step 3 of algorithm: calculates exp( -alpha * t)
  long double alpha_t = alpha * t;
  long double expat = exp(-alpha_t);
   
  if ( expat == 0 ) {
    std::string error = "Error: exp(-alpha * t) exceeds the range of a long double.";
    error += "\nThe Uniform Taylor method cannot solve the matrix exponential.";
    throw ValueError(error);
  }

  // step 4 of algorithm: initializes the next term Ck, the total sum of Ck
  // terms, and the previous term Ck-1
  // 
  // NOTE: the exponential term is included at the beginning of the sum so
  // that Ck_sum slowly gets larger as more terms are added, rather than
  // simply multiplying a very small number by a very big one at the end
  Vector C_next = expat * x_o;
  Vector Ck_sum = C_next;
  Vector C_prev = C_next;      

  // step 5 of algorithm: determines the maximum number of terms needed
  int maxTerms = MaxNumTerms(alpha_t, tol);

  // step 6 of algorithm: computes the sum of Ck terms until the maximum
  // number of terms has been reached 
  for ( int k = 1; k < maxTerms; ++k ) {
    // step 6a of algorithm: computes the next term in the series
    C_next = ( t / k ) * B;
    C_next *= C_prev;
  
    // step 6b of algorithm: updates the solution Ck_sum
    Ck_sum += C_next;       
  
    // step 6c of algorithm: resets the previous term for the next iteration
    C_prev = C_next;
  }
  
  // step 7 of algorithm: returns the solution for x_t
  return Ck_sum; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int UniformTaylor::MaxNumTerms(long double alpha_t, double epsilon) {
  long double nextTerm;           // stores the next term in the series     
  
  // initializes the previous term and the sum of terms in the series
  long double prevTerm = 1;      
  long double sumTerms = 1;      
  
  // calculates the lower bound of the series
  long double lowerBound = exp(alpha_t);

  // checks to see if exp(alpha * t) is infinite
  if ( lowerBound == HUGE_VAL ) {
    std::string error = "Error: exp(alpha * t) exceeds the range of a long double";
    error += "\nThe Uniform Taylor method cannot solve the matrix exponential.";
    throw ValueError(error);
  }

  lowerBound = lowerBound * (1 - epsilon);
  
  // computes the sum of terms until it is greater than the lower bound
  int p = 1;                 
  bool stopSum = false;

  while ( stopSum != true ) {
    // checks if the sum of terms is greater than the lower error bound
    if ( sumTerms >= lowerBound ) {
      stopSum = true;
    }
    else {
      // computes the next term in the series
      nextTerm = (alpha_t / p) * prevTerm;

      // adds the next term to the sum of terms and updates the total number
      // of terms in the series
      sumTerms += nextTerm;
      ++p;

      // resets the previous term for the next iteration of the loop
      prevTerm = nextTerm;
    }
  }

  return p;
}

} // namespace cyclus
