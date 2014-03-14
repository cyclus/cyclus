#include "test_inst.h"

extern "C" cyclus::Model* ConstructTestInst(cyclus::Context* ctx) {
  return new TestInst(ctx);
}
