#include "TestInst.h"

#include "Model.h"

extern "C" Model* constructTestInst() {
  return new TestInst();
}

extern "C" Model* destructTestInst(Model* model) {
  delete model;
}
