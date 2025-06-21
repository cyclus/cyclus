// economic_parameter.h
#ifndef ECONOMIC_PARAMETER_H_
#define ECONOMIC_PARAMETER_H_

#include <string>

enum class CostCategory {
  Depreciable,
  Fixed,      
  Variable,   
  Profit,     
  Tax,
  Time,        
  Override    
};

struct EconParameter {
  std::string name;
  double value;
  CostCategory category;
};

#endif  // ECONOMIC_PARAMETER_H_
