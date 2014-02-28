/// Instmodel.cc
// Implements the InstModel class

#include <iostream>
#include <sstream>
#include <string>

#include "inst_model.h"

#include "logger.h"
#include "timer.h"
#include "error.h"
#include "facility_model.h"
#include "query_engine.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InstModel::InstModel(Context* ctx) : Model(ctx) {
  model_type_ = "Inst";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InstModel::InitFrom(InstModel* m) {
  Model::InitFrom(m);
  this->prototypes_ = m->prototypes_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InstModel::InitFrom(QueryEngine* qe) {
  Model::InitFrom(qe);

  std::string name, query;
  int nEntries;

  // populate prototypes_
  query = "availableprototype";
  nEntries = qe->NElementsMatchingQuery(query);
  if (nEntries > 0) {
    // populate prototypes_
    for (int i = 0; i < nEntries; i++) {
      name = qe->GetElementContent(query, i);
      AddAvailablePrototype(name);
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InstModel::AddAvailablePrototype(std::string proto_name) {
  prototypes_.insert(proto_name);
  RegisterAvailablePrototype(proto_name);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InstModel::RegisterAvailablePrototype(std::string proto_name) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string InstModel::str() {
  if (parent() != NULL) {
    return Model::str() + " in region" + parent()->name();
  } else {
    return Model::str() + " with no region.";
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InstModel::Build(Model* parent) {
  Model::Build(parent);
  context()->RegisterTimeListener(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InstModel::Tock(int time) {
  std::vector<Model*> to_decomm;
  for (int i = 0; i < children().size(); i++) {
    FacilityModel* child = dynamic_cast<FacilityModel*>(children().at(i));
    if (child->LifetimeReached(time)) {
      CLOG(LEV_INFO3) << child->name() << " has reached the end of its lifetime";
      if (child->CheckDecommissionCondition()) {
        to_decomm.push_back(child);
      }
    }
  }

  while (!to_decomm.empty()) {
    Model* child = to_decomm.back();
    to_decomm.pop_back();
    context()->SchedDecom(child);
  }
}

} // namespace cyclus
