#include "stub_region.h"

using stubs::StubRegion;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubRegion::StubRegion(cyclus::Context* ctx)
    : cyclus::RegionModel(ctx)
    {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubRegion::~StubRegion() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubRegion::InitModuleMembers(cyclus::QueryEngine* qe) {
  //retrieve input data members here. For example :  
  //string query = "incommodity";
  //incommodity_ = lexical_cast<double>(qe->getElementContent(query));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* StubRegion::Clone() {
  StubRegion* m = new StubRegion(*this);

  // required to copy parent class members
  m->InitFrom(this);

  // clone specific members for your module, e.g.
  // m->SetSomeMember(someMember());

  return m;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::string StubRegion::str() {
  return RegionModel::str();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubRegion::ReceiveMessage(cyclus::Message::Ptr msg) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Model* ConstructStubRegion(cyclus::Context* ctx) {
  return new StubRegion(ctx);
}
