#include "stub_facility.h"

namespace stubs {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubFacility::StubFacility(cyclus::Context* ctx) : cyclus::Facility(ctx) {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string StubFacility::str() {
  return Facility::str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubFacility::Tick() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubFacility::Tock() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Agent* ConstructStubFacility(cyclus::Context* ctx) {
  return new StubFacility(ctx);
}

}  // namespace stubs
