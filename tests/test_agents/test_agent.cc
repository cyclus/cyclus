#include "test_agent.h"

namespace cyclus {

extern "C" cyclus::Agent* ConstructTestAgent(cyclus::Context* ctx) {
  return new TestAgent(ctx);
}

} // namespace cyclus

// required to get functionality in cyclus agent unit tests library
#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif  // CYCLUS_AGENT_TESTS_CONNECTED
