#include "SymbolicFunctions.h"

#include <math.h>
#include <string>
#include <sstream>
#include <limits>

//using namespace boost;

namespace cyclus {

// -------------------------------------------------------------------
double LinearFunction::value(double x) 
{ 
  return slope_ * x + intercept_; 
}

// -------------------------------------------------------------------
std::string LinearFunction::Print() 
{ 
  std::stringstream ss("");
  ss << "y = " << slope_ << " * x + " << intercept_;
  return ss.str();
}

// -------------------------------------------------------------------
double ExponentialFunction::value(double x) 
{ 
  return constant_ * exp(exponent_ * x) + intercept_; 
}

// -------------------------------------------------------------------
std::string ExponentialFunction::Print() 
{ 
  std::stringstream ss("");
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
      std::list<PiecewiseFunctionInfo>::iterator f = functions_.begin();
      while ( f != functions_.end() && (x >= f->xoffset) ) ++f; // exceeds search by 1
      --f; // go back to the correct one
      ret = f->function->value(x - f->xoffset) + f->yoffset;
    }
  return ret;
}
  
// -------------------------------------------------------------------
std::string PiecewiseFunction::Print() 
{ 
  std::stringstream ss("");
  ss << "Piecewise Function comprised of: ";
  std::list<PiecewiseFunctionInfo>::iterator f;
  for (f = functions_.begin(); f != functions_.end(); f++)
    {
      ss << " * " << f->function->Print() 
         << " starting at coordinate (" << f->xoffset << "," 
         << f->yoffset << ")";
    }
  return ss.str();
}
} // namespace cyclus
