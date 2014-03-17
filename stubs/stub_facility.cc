#include "stub_facility.h"

using stubs::StubFacility;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubFacility::StubFacility(cyclus::Context* ctx)
    : cyclus::Facility(ctx) {};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubFacility::~StubFacility() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubFacility::InfileToDb(cyclus::QueryEngine* qe, cyclus::DbInit di) {
  Agent::InfileToDb(qe, di);
  qe = qe->QueryElement(model_impl());
  // retrieve input data members here. For example:
  //
  //   int cycle_len = lexical_cast<int>(input->getElementContent("cycle_length"));
  //   ...
  //   di.NewDatum("StubFacilityParams")
  //     ->AddVal("cycle_length", cycle_len)
  //     ...
  //     ->Record();
}

void StubFacility::InitInv(cyclus::Inventories& inv) {
  // populate all resource containers with resources in inv
}

cyclus::Inventories StubFacility::SnapshotInv() {
  cyclus::Inventories invs;
  // populate the inventories map of container-name to groups of resources for
  // every container in this agent
  return invs;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Agent* StubFacility::Clone() {
  StubFacility* m = new StubFacility(context());
  m->InitFrom(this);
  return m;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubFacility::InitFrom(StubFacility* m) {
  cyclus::Facility::InitFrom(m);
  // Initialize stubfacility members for a cloned module here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string StubFacility::str() {
  return Facility::str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubFacility::Tick(int time) {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubFacility::Tock(int time) {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Agent* ConstructStubFacility(cyclus::Context* ctx) {
  return new StubFacility(ctx);
}
