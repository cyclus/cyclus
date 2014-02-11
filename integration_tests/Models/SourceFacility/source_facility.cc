#include "source_facility.h"

using cyclus::SourceFacility;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SourceFacility::SourceFacility(cyclus::Context* ctx)
    : cyclus::FacilityModel(ctx),
      cyclus::Model(ctx)
    {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SourceFacility::~SourceFacility() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SourceFacility::InitFrom(cyclus::QueryEngine* qe) {
  cyclus::FacilityModel::InitFrom(qe);
  qe = qe->QueryElement(ModelImpl());

  cyclus::QueryEngine* input = qe->QueryElement("input");
  //retrieve input data members here. For example :
  //string query = "incommodity";
  //incommodity_ = lexical_cast<double>(input->getElementContent(query));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* SourceFacility::Clone() {
  SourceFacility* m = new SourceFacility(context());
  m->InitFrom(this);
  return m;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SourceFacility::InitFrom(SourceFacility* m) {
  cyclus::FacilityModel::InitFrom(m);
  // Initialize sourcefacility members for a cloned module here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string SourceFacility::str() {
  return FacilityModel::str();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SourceFacility::Tick(int time){}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SourceFacility::Tock(int time){}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Model* ConstructSourceFacility(cyclus::Context* ctx) {
  return new SourceFacility(ctx);
}
