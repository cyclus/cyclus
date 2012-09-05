#include "TestFacility.h"

#include "Model.h"

extern "C" Model* constructor() {
  return new TestFacility();
}

extern "C" Model* destructor(Model* model) {
  delete model;
}
