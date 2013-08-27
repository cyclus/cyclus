#include "test_inst.h"

#include "model.h"

extern "C" cyclus::Model* constructTestInst(cyclus::Context* ctx) {
  return new TestInst(ctx);
}

extern "C" cyclus::Model* destructTestInst(cyclus::Model* model) {
  delete model;
}
