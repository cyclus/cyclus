#include "test_gisfac.h"

namespace cyclus {

extern "C" cyclus::GISFac* ConstructTestGISFac(cyclus::Context* ctx) {
  return new TestGISFac(ctx);
}

}  // namespace cyclus

// required to get functionality in cyclus gisfac unit tests library
#ifndef CYCLUS_GISFAC_TESTS_CONNECTED
int ConnectGISFacTests();
static int cyclus_gisfac_tests_connected = ConnectGISFacTests();
#define CYCLUS_GISFAC_TESTS_CONNECTED cyclus_gisfac_tests_connected
#endif  // CYCLUS_GISFAC_TESTS_CONNECTED
