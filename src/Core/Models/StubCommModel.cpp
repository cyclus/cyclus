// StubCommModel.cpp
// Implements the StubCommModel class

#include "StubCommModel.h"

#include "Logger.h"

namespace cyclus {
  
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubCommModel::StubCommModel() { 
  SetModelType("StubComm");
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubCommModel::~StubCommModel() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string StubCommModel::str() { 
  return Model::str() + ""; 
};

/* ------------------- */ 
} // namespace cyclus
