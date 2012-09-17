#include "TestRegion.h"

#include "Model.h"

extern "C" Model* constructTestRegion() {
  return new TestRegion();
}

extern "C" Model* destructTestRegion(Model* model) {
  delete model;
}
