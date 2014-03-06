#include "stub_region.h"

using stubs::StubRegion;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubRegion::StubRegion(cyclus::Context* ctx)
    : cyclus::RegionModel(ctx) {};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubRegion::~StubRegion() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubRegion::InfileToDb(cyclus::QueryEngine* qe, cyclus::DbInit di) {
  Model::InfileToDb(qe, di);
  qe = qe->QueryElement(ModelImpl());
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
cyclus::Model* StubRegion::Clone() {
  StubRegion* m = new StubRegion(context());
  m->InitFrom(this);
  return m;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubRegion::InitFrom(StubRegion* m) {
  cyclus::RegionModel::InitFrom(m);
  // Initialize stubregion members for a cloned module here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string StubRegion::str() {
  return RegionModel::str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Model* ConstructStubRegion(cyclus::Context* ctx) {
  return new StubRegion(ctx);
}
