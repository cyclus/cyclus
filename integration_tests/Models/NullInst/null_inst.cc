#include "null_inst.h"

using cyclus::NullInst;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NullInst::NullInst(cyclus::Context* ctx)
    : cyclus::InstModel(ctx),
      cyclus::Model(ctx)
    {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NullInst::~NullInst() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullInst::InitFrom(cyclus::QueryEngine* qe) {
  cyclus::InstModel::InitFrom(qe);
  qe = qe->QueryElement(ModelImpl());
  
  //retrieve input data members here. For example :  
  //string query = "incommodity";
  //incommodity_ = lexical_cast<double>(qe->getElementContent(query));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* NullInst::Clone() {
  NullInst* m = new NullInst(context());
  m->InitFrom(this);
  return m;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullInst::InitFrom(NullInst* m) {
  cyclus::InstModel::InitFrom(m);
  // Initialize stubinst members for a cloned module here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::string NullInst::str() {
  return InstModel::str();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Model* ConstructNullInst(cyclus::Context* ctx) {
  return new NullInst(ctx);
}
