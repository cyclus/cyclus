#include "TestRegion.h"

#include "Model.h"

extern "C" Model* constructor() {
  return new TestRegion();
}

extern "C" Model* destructor(Model* model) {
  delete model;
}
