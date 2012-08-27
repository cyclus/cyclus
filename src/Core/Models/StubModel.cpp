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
void StubModel::init(QueryEngine* qe,std::string cur_ns) { 
  Model::init(qe,cur_ns); 
};
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string StubModel::str() { 
  return Model::str() + ""; 
};

/* ------------------- */ 
