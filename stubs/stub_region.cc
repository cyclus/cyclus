#include "stub_region.h"

using stubs::StubRegion;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubRegion::StubRegion(cyclus::Context* ctx) : cyclus::Region(ctx) {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubRegion::~StubRegion() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string StubRegion::str() {
  return Region::str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Agent* ConstructStubRegion(cyclus::Context* ctx) {
  return new StubRegion(ctx);
}
