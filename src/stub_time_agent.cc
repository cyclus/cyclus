// stub_time_agent.cc
// Implements the StubModel class

#include "stub_time_agent.h"

#include "logger.h"

namespace cyclus {

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubTimeAgent::StubTimeAgent(Context* ctx) : TimeAgent(ctx) {
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
