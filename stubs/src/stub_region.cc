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

// WARNING! Do not change the following this function!!! This enables your
// archetype to be dynamically loaded and any alterations will cause your
// archetype to fail.
extern "C" cyclus::Agent* ConstructStubRegion(cyclus::Context* ctx) {
  return new StubRegion(ctx);
}
