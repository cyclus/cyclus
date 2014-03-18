#include "stub_region.h"

using stubs::StubRegion;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubRegion::StubRegion(cyclus::Context* ctx)
    : cyclus::Region(ctx) {};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubRegion::~StubRegion() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubRegion::InfileToDb(cyclus::InfileTree* qe, cyclus::DbInit di) {
  Agent::InfileToDb(qe, di);
  qe = qe->SubTree(agent_impl());
  // retrieve input data members here. For example:
  //
  //   int cycle_len = lexical_cast<int>(input->getElementContent("cycle_length"));
  //   ...
  //   di.NewDatum("StubFacilityParams")
  //     ->AddVal("cycle_length", cycle_len)
  //     ...
  //     ->Record();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Agent* StubRegion::Clone() {
  StubRegion* m = new StubRegion(context());
  m->InitFrom(this);
  return m;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubRegion::InitFrom(StubRegion* m) {
  cyclus::Region::InitFrom(m);
  // Initialize stubregion members for a cloned module here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string StubRegion::str() {
  return Region::str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Agent* ConstructStubRegion(cyclus::Context* ctx) {
  return new StubRegion(ctx);
}
