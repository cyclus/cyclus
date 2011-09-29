// StubCommModel.cpp
// Implements the StubCommModel class

#include "StubCommModel.h"

#include <iostream>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubCommModel::StubCommModel() { 
  setModelType("StubComm");
  commType_ = STUB_COMM;
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

