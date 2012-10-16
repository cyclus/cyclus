#include "SymbolicFunctions.h"

#include <math.h>
#include <string>
#include <sstream>
#include <limits>

using namespace std;
using namespace boost;

// -------------------------------------------------------------------
double LinearFunction::value(double x) 
{ 
  return slope_ * x + intercept_; 
}

// -------------------------------------------------------------------
std::string LinearFunction::print() 
{ 
  stringstream ss("");
  ss << "y = " << slope_ << " * x + " << intercept_;
  return ss.str();
}

// -------------------------------------------------------------------
double ExponentialFunction::value(double x) 
{ 
  return constant_ * exp(exponent_ * x) + intercept_; 
}

// -------------------------------------------------------------------
std::string ExponentialFunction::print() 
{ 
  stringstream ss("");
  ss << "y = " << constant_ 
     <<" * exp(" << exponent_ << " * x) + " << intercept_;
  return ss.str();
}

// -------------------------------------------------------------------
double PiecewiseFunction::value(double x) 
{
  double ret;
  if ( functions_.empty() || ( x < functions_.front().xoffset) ) 
    {
      ret = 0.0;
    }
  else 
    {
      list<PiecewiseFunctionInfo>::iterator f = functions_.begin();
      while ( f != functions_.end() && (x >= f->xoffset) ) ++f; // exceeds search by 1
      --f; // go back to the correct one
      ret = f->function->value(x - f->xoffset) + f->yoffset;
    }
  return ret;
}
  
// -------------------------------------------------------------------
std::string PiecewiseFunction::print() 
{ 
  stringstream ss("");
  ss << "Piecewise Function comprised of: ";
  list<PiecewiseFunctionInfo>::iterator f;
  for (f = functions_.begin(); f != functions_.end(); f++)
    {
      ss << " * " << f->function->print() 
         << " starting at coordinate (" << f->xoffset << "," 
         << f->yoffset << ")";
    }
  return ss.str();
}
