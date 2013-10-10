#include "stub_market.h"

using cyclus::stubs::StubMarket;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubMarket::StubMarket(cyclus::Context* ctx)
    : cyclus::MarketModel(ctx)
    {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubMarket::~StubMarket() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubMarket::InitModuleMembers(cyclus::QueryEngine* qe) {
  //retrieve input data members here. For example :  
  //string query = "incommodity";
  //incommodity_ = lexical_cast<double>(qe->getElementContent(query));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* StubMarket::Clone() {
  StubMarket* m = new StubMarket(*this);

  // required to copy parent class members
  m->InitFrom(this);

  // clone specific members for your module, e.g.
  // m->SetSomeMember(someMember());

  return m;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::string StubMarket::str() {
  return MarketModel::str();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubMarket::Resolve() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubMarket::ReceiveMessage(cyclus::Message::Ptr msg) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Model* ConstructStubMarket(cyclus::Context* ctx) {
  return new StubMarket(ctx);
}
