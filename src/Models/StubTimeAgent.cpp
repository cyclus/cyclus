// StubTimeAgent.cpp
// Implements the StubModel class

#include "StubTimeAgent.h"

#include <iostream>
#include "Logger.h"

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
  LOG(LEV_DEBUG2); 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubTimeAgent::handlePreHistory() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubTimeAgent::handleTick(int time) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubTimeAgent::handleTock(int time) {}

