#include "stub_region.h"

using stubs::StubRegion;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubRegion::StubRegion(cyclus::Context* ctx)
    : cyclus::RegionModel(ctx) {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubRegion::~StubRegion() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubRegion::InitFrom(cyclus::QueryEngine* qe) {
  cyclus::RegionModel::InitFrom(qe);
  qe = qe->QueryElement(ModelImpl());
  
  //retrieve input data members here. For example :  
  //string query = "incommodity";
  //incommodity_ = lexical_cast<double>(qe->getElementContent(query));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* StubRegion::Clone() {
  StubRegion* m = new StubRegion(context());
  m->InitFrom(this);
  return m;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubRegion::InitFrom(StubRegion* m) {
  cyclus::RegionModel::InitFrom(m);
  // Initialize stubregion members for a cloned module here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::string StubRegion::str() {
  return RegionModel::str();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Model* ConstructStubRegion(cyclus::Context* ctx) {
  return new StubRegion(ctx);
}
