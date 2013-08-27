#include "test_region.h"

#include "model.h"

extern "C" cyclus::Model* constructTestRegion(cyclus::Context* ctx) {
  return new TestRegion(ctx);
}

extern "C" cyclus::Model* destructTestRegion(cyclus::Model* model) {
  delete model;
}
