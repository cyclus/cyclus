// uniform_taylor.h
#ifndef UNIFORMTAYLOR_H
#define UNIFORMTAYLOR_H

#include "use_matrix_lib.h"

namespace cyclus {

/**
   @class UniformTaylor 
    
   A class that solves the matrix exponential 
   problem using the Taylor Series with Uniformization method. 
 */
class UniformTaylor {

 public:
  /**
     Solves the matrix exponential problem: 
      
     dx(t) 
     -----  =  A * x(t) 
     dt 
        
     where A is an nxn Matrix and x(t) is an nx1 Vector.  The solution 
     to this equation can be determined by calculating: 
      
     x(t) = e^(tA) * x(t=0) 
        
     @param A the Matrix 
     @param x_o the initial condition Vector x(t=0) 
     @param t the value for which the solution is being evaluated 
     @return the solution Vector x(t) 
     @throw <string> if the Uniform Taylor method cannot be used 
   */
  static Vector MatrixExpSolver(const Matrix & A, const Vector & x_o, const double t);

 private:
  /**
     Returns the diagonal element in the Matrix A 
     that has the largest absolute value. 
        
     @param A the Matrix 
     @return the diagonal element of A with the largest absolute value 
   */
  static double MaxAbsDiag(const Matrix & A);

  /**
     Computes the solution Vector x_t using the Taylor Series with 
     Uniformization method. 
        
     @param B the Matrix B = A + alpha * I 
     @param x_o the initial condition Vector 
     @param alpha the diagonal element of A with the largest absolute 
     value @param t the value for which the solution is being 
     evaluated @param tol the accuracy desired 
     @return the solution Vector x_t for the given value of t 
     @throw <string> if exp(-alpha * t) or exp(alpha * t) exceeds 
   */
  static Vector GetSolutionVector(const Matrix & B,
      const Vector & x_o,
		  double alpha,
			double t,
			double tol);

  /**
     Computes the maximum number of terms needed to obtain an accuracy 
     of epsilon when using the Taylor Series with Uniformization 
     method.  
     @param alpha_t the product alpha * t 
     @param epsilon the accuracy desired in the series computation 
     @return the maximum number of terms needed 
     @throw <string> if exp(alpha * t) exceeds range 
   */
  static int MaxNumTerms(long double alpha_t, double epsilon);
};
} // namespace cyclus
#endif
