// StubFacility.cpp
// Implements the StubFacility class

#include "stub_facility.h"

using stubs::StubFacility;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubFacility::StubFacility(cyclus::Context* ctx)
    : cyclus::FacilityModel(ctx)
    {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubFacility::~StubFacility() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubFacility::InitModuleMembers(cyclus::QueryEngine* qe) {
  cyclus::QueryEngine* input = qe->QueryElement("input");
  //retrieve input data members here. For example :  
  //string query = "incommodity";
  //incommodity_ = lexical_cast<double>(input->getElementContent(query));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* StubFacility::Clone() {
  StubFacility* m = new StubFacility(*this);

  // required to copy parent class members
  m->InitFrom(this);

  // clone specific members for your module, e.g.
  // m->SetSomeMember(someMember());

  return m;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::string StubFacility::str() {
  return FacilityModel::str();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubFacility::ReceiveMessage(cyclus::Message::Ptr msg) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<cyclus::Resource::Ptr>
StubFacility::RemoveResource(cyclus::Transaction order) {}
    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubFacility::AddResource(cyclus::Transaction trans,
                               std::vector<cyclus::Resource::Ptr> manifest){}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubFacility::HandleTick(int time){}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubFacility::HandleTock(int time){}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Model* ConstructStubFacility(cyclus::Context* ctx) {
  return new StubFacility(ctx);
}
