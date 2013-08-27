#include "test_facility.h"

#include "model.h"

extern "C" cyclus::Model* constructTestFacility(cyclus::Context* ctx) {
  return new TestFacility(ctx);
}

extern "C" cyclus::Model* destructTestFacility(cyclus::Model* model) {
  delete model;
}
