// StubCommModel.cpp
// Implements the StubCommModel class

#include "StubCommModel.h"

#include "Logger.h"
  
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubCommModel::StubCommModel() { 
  setModelType("StubComm");
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubCommModel::~StubCommModel() {};
    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubCommModel::init(xmlNodePtr cur) { 
  Model::init(cur); 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string StubCommModel::str() { 
  return Model::str() + ""; 
};

/* ------------------- */ 
