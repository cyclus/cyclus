#include "test_region.h"

extern "C" cyclus::Model* ConstructTestRegion(cyclus::Context* ctx) {
  return new TestRegion(ctx);
}
