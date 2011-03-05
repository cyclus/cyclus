// StubCommModel.cpp
// Implements the StubCommModel class

#include "StubCommModel.h"

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubCommModel::StubCommModel() { 
  ID = nextID++; 
  model_type="StubComm";
  commType = StubComm;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubCommModel::~StubCommModel() {};
    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubCommModel::init(xmlNodePtr cur) { 
  Model::init(cur); 
};
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubCommModel::copy(StubCommModel* src) { 
  Model::copy(src);
  Communicator::copy(src); 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubCommModel::print() { 
  Model::print(); 
  cout << endl; 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Initialize the StubCommModel nextID to zero.
int StubCommModel::nextID = 0;

