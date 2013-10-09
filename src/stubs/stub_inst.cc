#include "stub_inst.h"

using cyclus::stubs::StubInst;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubInst::StubInst(cyclus::Context* ctx)
    : cyclus::InstModel(ctx)
    {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubInst::~StubInst() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubInst::InitModuleMembers(cyclus::QueryEngine* qe) {
  //retrieve input data members here. For example :  
  //string query = "incommodity";
  //incommodity_ = lexical_cast<double>(qe->getElementContent(query));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* StubInst::Clone() {
  StubInst* m = new StubInst(*this);

  // required to copy parent class members
  m->InitFrom(this);

  // clone specific members for your module, e.g.
  // m->SetSomeMember(someMember());

  return m;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::string StubInst::str() {
  return InstModel::str();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubInst::ReceiveMessage(cyclus::Message::Ptr msg) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Model* ConstructStubInst(cyclus::Context* ctx) {
  return new StubInst(ctx);
}
