#include "test_facility.h"

extern "C" cyclus::Model* ConstructTestFacility(cyclus::Context* ctx) {
  return new TestFacility(ctx);
}
