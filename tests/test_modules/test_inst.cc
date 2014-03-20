#include "test_inst.h"

extern "C" cyclus::Agent* ConstructTestInst(cyclus::Context* ctx) {
  return new TestInst(ctx);
}
