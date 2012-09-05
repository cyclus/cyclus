#include "TestInst.h"

#include "Model.h"

extern "C" Model* constructor() {
  return new TestInst();
}

extern "C" Model* destructor(Model* model) {
  delete model;
}
