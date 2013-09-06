#include "test_region.h"

#include "model.h"

extern "C" cyclus::Model* ConstructTestRegion(cyclus::Context* ctx) {
  return new TestRegion(ctx);
}

extern "C" cyclus::Model* DestructTestRegion(cyclus::Model* model) {
  delete model;
}
