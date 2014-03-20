#include "test_region.h"

extern "C" cyclus::Agent* ConstructTestRegion(cyclus::Context* ctx) {
  return new TestRegion(ctx);
}
