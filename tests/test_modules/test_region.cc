#include "test_region.h"

#include "model.h"

extern "C" cyclus::Model* ConstructTestRegion(cyclus::Context* ctx) {
  return new TestRegion(ctx);
}
