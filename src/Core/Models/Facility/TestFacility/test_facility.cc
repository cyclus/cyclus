#include "test_facility.h"

#include "model.h"

extern "C" cyclus::Model* constructTestFacility() {
  return new TestFacility();
}

extern "C" cyclus::Model* destructTestFacility(cyclus::Model* model) {
  delete model;
}
