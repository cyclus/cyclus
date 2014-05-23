#include "null_inst.h"

namespace cyclus {

NullInst::NullInst(cyclus::Context* ctx) : cyclus::Institution(ctx) {}

NullInst::~NullInst() {}

extern "C" cyclus::Agent* ConstructNullInst(cyclus::Context* ctx) {
  return new NullInst(ctx);
}

}  // namespace cyclus

// required to get functionality in cyclus agent unit tests library
#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif // CYCLUS_AGENT_TESTS_CONNECTED
