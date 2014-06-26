#include "stub_inst.h"

using stubs::StubInst;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubInst::StubInst(cyclus::Context* ctx) : cyclus::Institution(ctx) {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubInst::~StubInst() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string StubInst::str() {
  return Institution::str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Agent* ConstructStubInst(cyclus::Context* ctx) {
  return new StubInst(ctx);
}
