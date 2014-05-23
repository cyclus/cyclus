#include "null_region.h"

namespace cyclus {

NullRegion::NullRegion(cyclus::Context* ctx) : cyclus::Region(ctx) {}

NullRegion::~NullRegion() {}

extern "C" cyclus::Agent* ConstructNullRegion(cyclus::Context* ctx) {
  return new NullRegion(ctx);
}

}  // namespce cyclus

// required to get functionality in cyclus agent unit tests library
#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif // CYCLUS_AGENT_TESTS_CONNECTED
