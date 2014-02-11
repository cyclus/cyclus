#include "stub_inst.h"

using stubs::StubInst;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubInst::StubInst(cyclus::Context* ctx)
    : cyclus::InstModel(ctx),
      cyclus::Model(ctx)
    {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubInst::~StubInst() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubInst::InitFrom(cyclus::QueryEngine* qe) {
  cyclus::InstModel::InitFrom(qe);
  qe = qe->QueryElement(ModelImpl());
  
  //retrieve input data members here. For example :  
  //string query = "incommodity";
  //incommodity_ = lexical_cast<double>(qe->getElementContent(query));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* StubInst::Clone() {
  StubInst* m = new StubInst(context());
  m->InitFrom(this);
  return m;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubInst::InitFrom(StubInst* m) {
  cyclus::InstModel::InitFrom(m);
  // Initialize stubinst members for a cloned module here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::string StubInst::str() {
  return InstModel::str();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Model* ConstructStubInst(cyclus::Context* ctx) {
  return new StubInst(ctx);
}
