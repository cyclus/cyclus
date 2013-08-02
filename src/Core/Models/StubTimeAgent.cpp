// StubTimeAgent.cpp
// Implements the StubModel class

#include "StubTimeAgent.h"

#include "Logger.h"

namespace cyclus {

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubTimeAgent::StubTimeAgent() { 
  setModelType("Stub");
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubTimeAgent::~StubTimeAgent() {};
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string StubTimeAgent::str() { 
  return TimeAgent::str() + ""; 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubTimeAgent::handleTick(int time) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubTimeAgent::handleTock(int time) {}

/* ------------------- */ 

} // namespace cyclus
