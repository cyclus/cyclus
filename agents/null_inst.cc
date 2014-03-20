#include "null_inst.h"

namespace cyclus {

NullInst::NullInst(cyclus::Context* ctx)
    : cyclus::InstAgent(ctx) {}

NullInst::~NullInst() {}

extern "C" cyclus::Agent* ConstructNullInst(cyclus::Context* ctx) {
  return new NullInst(ctx);
}

}  // namespace cyclus
