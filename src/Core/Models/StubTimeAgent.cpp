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
  SetModelType("Stub");
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubTimeAgent::~StubTimeAgent() {};
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string StubTimeAgent::str() { 
  return TimeAgent::str() + ""; 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubTimeAgent::HandleTick(int time) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubTimeAgent::HandleTock(int time) {}

/* ------------------- */ 

} // namespace cyclus
