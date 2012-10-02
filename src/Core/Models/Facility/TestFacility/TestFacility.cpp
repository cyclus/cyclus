#include "TestFacility.h"

#include "Model.h"

extern "C" Model* constructTestFacility() {
  return new TestFacility();
}

extern "C" Model* destructTestFacility(Model* model) {
  delete model;
}
