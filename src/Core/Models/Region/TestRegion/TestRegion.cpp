#include "TestRegion.h"

#include "Model.h"

extern "C" cyclus::Model* constructTestRegion() {
  return new TestRegion();
}

extern "C" cyclus::Model* destructTestRegion(cyclus::Model* model) {
  delete model;
}
