#include "sink_facility.h"

using cyclus::SinkFacility;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SinkFacility::SinkFacility(cyclus::Context* ctx)
    : cyclus::FacilityModel(ctx),
      cyclus::Model(ctx)
    {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SinkFacility::~SinkFacility() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::InitFrom(cyclus::QueryEngine* qe) {
  cyclus::FacilityModel::InitFrom(qe);
  qe = qe->QueryElement(ModelImpl());
  
  cyclus::QueryEngine* input = qe->QueryElement("input");
  //retrieve input data members here. For example :  
  //string query = "incommodity";
  //incommodity_ = lexical_cast<double>(input->getElementContent(query));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* SinkFacility::Clone() {
  SinkFacility* m = new SinkFacility(context());
  m->InitFrom(this);
  return m;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::InitFrom(SinkFacility* m) {
  cyclus::FacilityModel::InitFrom(m);
  // Initialize sinkfacility members for a cloned module here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::string SinkFacility::str() {
  return FacilityModel::str();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SinkFacility::Tick(int time){}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SinkFacility::Tock(int time){}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Model* ConstructSinkFacility(cyclus::Context* ctx) {
  return new SinkFacility(ctx);
}
