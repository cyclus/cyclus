// Regionmodel.cc
// Implements the RegionModel class

#include <string>
#include <iostream>

#include "region_model.h"

#include "model.h"
#include "inst_model.h"
#include "error.h"
#include "timer.h"
#include "logger.h"
#include "query_engine.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RegionModel::InitFrom(RegionModel* m) {
  Model::InitFrom(m);
  this->allowedFacilities_ = m->allowedFacilities_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RegionModel::RegionModel(Context* ctx) : Model(ctx) {
  kind_ = "Region";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RegionModel::InitFrom(QueryEngine* qe) {
  Model::InitFrom(qe); // name_
  RegionModel::InitAllowedFacilities(qe); // allowedFacilities_
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RegionModel::InitAllowedFacilities(QueryEngine* qe) {
  int num_allowed_fac = qe->NElementsMatchingQuery("allowedfacility");
  std::string proto_name;
  for (int i = 0; i < num_allowed_fac; i++) {
    proto_name = qe->GetElementContent("allowedfacility", i);
    allowedFacilities_.insert(proto_name);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RegionModel::Build(Model* parent) {
  Model::Build(parent);
  context()->RegisterTimeListener(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string RegionModel::str() {
  std::string s = Model::str();

  s += "allows facs: ";
  for (std::set<std::string>::iterator fac = allowedFacilities_.begin();
       fac != allowedFacilities_.end();
       fac++) {
    s += *fac + ", ";
  }

  s += ". And has insts: ";
  for (std::vector<Model*>::const_iterator inst = children().begin();
       inst != children().end();
       inst++) {
    s += (*inst)->prototype() + ", ";
  }
  return s;
}

} // namespace cyclus
