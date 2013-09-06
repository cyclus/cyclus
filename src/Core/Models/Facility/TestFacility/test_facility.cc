#include "test_facility.h"

#include "model.h"

extern "C" cyclus::Model* ConstructTestFacility(cyclus::Context* ctx) {
  return new TestFacility(ctx);
}

extern "C" cyclus::Model* DestructTestFacility(cyclus::Model* model) {
  delete model;
}
