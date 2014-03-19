#include "test_region.h"

//#pragma cyclus def

extern "C" cyclus::Agent* ConstructTestRegion(cyclus::Context* ctx) {
  return new TestRegion(ctx);
}
