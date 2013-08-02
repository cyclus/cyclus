#include "TestInst.h"

#include "Model.h"

extern "C" cyclus::Model* constructTestInst() {
  return new TestInst();
}

extern "C" cyclus::Model* destructTestInst(cyclus::Model* model) {
  delete model;
}
