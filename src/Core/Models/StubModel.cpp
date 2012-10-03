// StubModel.cpp
// Implements the StubModel class

#include "StubModel.h"

#include "Logger.h"

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubModel::StubModel() { 
  setModelType("Stub");
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubModel::~StubModel() {};
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string StubModel::str() { 
  return Model::str() + ""; 
};

/* ------------------- */ 
