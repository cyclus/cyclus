#include "test_facility.h"

extern "C" cyclus::Agent* ConstructTestFacility(cyclus::Context* ctx) {
  return new TestFacility(ctx);
}
