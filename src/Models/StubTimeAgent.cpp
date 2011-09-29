// StubTimeAgent.cpp
// Implements the StubModel class

#include "StubTimeAgent.h"

#include <iostream>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubTimeAgent::StubTimeAgent() { 
  setModelType("Stub");
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubTimeAgent::~StubTimeAgent() {};
    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubTimeAgent::init(xmlNodePtr cur) { 
  Model::init(cur); 
};
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubTimeAgent::copy(StubTimeAgent* src) { 
  TimeAgent::copy(src); 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubTimeAgent::print() { 
  TimeAgent::print(); 
  cout << endl; 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubTimeAgent::handlePreHistory() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubTimeAgent::handleTick(int time) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubTimeAgent::handleTock(int time) {}

