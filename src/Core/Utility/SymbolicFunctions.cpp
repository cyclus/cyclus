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
PiecewiseFunction::PiecewiseFunction(const FunctionPtr& function, const Point& point) :
  function_(function), 
  init_point_(point), 
  rhs_(numeric_limits<double>::max()) 
{}

// -------------------------------------------------------------------
PiecewiseFunction::PiecewiseFunction(const FunctionPtr& function) :
  function_(function), 
  init_point_(Point(0.0,0.0)), 
  rhs_(numeric_limits<double>::max()) 
{}

// -------------------------------------------------------------------
double PiecewiseFunction::value(double x) 
{ 
  double value;
  if (x < lhs() || x > rhs()) value = 0.0;
  else value = function_->value(x - init_point_.x) + init_point_.y;
  return value;
}

// -------------------------------------------------------------------
std::string PiecewiseFunction::print() 
{ 
  stringstream ss("");
  ss << function_->print() 
     << " valid on [" << init_point_.x << "," << rhs_ << "]"
     << " starting at coordinate (" 
     << init_point_.x << "," << init_point_.y << ")";
  return ss.str();
}

// -------------------------------------------------------------------
double PiecewiseFunction::lhs() 
{
  return init_point_.x;
}

// -------------------------------------------------------------------
double PiecewiseFunction::rhs() 
{
  return rhs_;
}

// -------------------------------------------------------------------
PiecewiseFunctionSeries::PiecewiseFunctionSeries()
{
  Point neg_inf(-1*numeric_limits<double>::max(),0.0);
  FunctionPtr flat_line = FunctionPtr(new LinearFunction(0.0,0.0));
  shared_ptr<PiecewiseFunction> seed = shared_ptr<PiecewiseFunction>(new PiecewiseFunction(flat_line,neg_inf));
  appendFunction(seed);
}

// -------------------------------------------------------------------
void PiecewiseFunctionSeries::appendFunction(const boost::shared_ptr<PiecewiseFunction>& function)
{
  if (!functions_.empty())
    {
      list< shared_ptr<PiecewiseFunction> >::iterator 
        last = --functions_.end();
      
      if (function->lhs() <= (*last)->lhs())
        {
          //throw PiecewiseFunctionOrderException(*last,function);
        }
    }
  functions_.push_back(function);
}

// -------------------------------------------------------------------
double PiecewiseFunctionSeries::value(double x) 
{
  list< shared_ptr<PiecewiseFunction> >::iterator f = functions_.begin();
  while ( f != functions_.end() && x >= (*f)->lhs() ) ++f; // exceeds search by 1
  --f; // go back to the correct one
  return (*f)->value(x);
}

// -------------------------------------------------------------------
std::string PiecewiseFunctionSeries::print() 
{ 
  stringstream ss("");
  ss << "Series comprised of: ";
  list< shared_ptr<PiecewiseFunction> >::iterator f;
  for (f = functions_.begin(); f != functions_.end(); f++)
    {
      ss << " * " << (*f)->print();
    }
  return ss.str();
}
