#include "test_inst.h"

#include "model.h"

extern "C" cyclus::Model* ConstructTestInst(cyclus::Context* ctx) {
  return new TestInst(ctx);
}

extern "C" cyclus::Model* DestructTestInst(cyclus::Model* model) {
  delete model;
}
