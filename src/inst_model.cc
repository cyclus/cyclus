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
  this->initial_build_order_ = m->initial_build_order_;
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

  query = "initialfacilitylist";
  nEntries = qe->NElementsMatchingQuery(query);
  // populate initial_build_order_
  if (nEntries > 0) {
    QueryEngine* list = qe->QueryElement(query);
    int numInitFacs = list->NElementsMatchingQuery("entry");
    for (int i = 0; i < numInitFacs; i++) {
      QueryEngine* entry = list->QueryElement("entry", i);
      AddPrototypeToInitialBuild(entry);
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
void InstModel::AddPrototypeToInitialBuild(QueryEngine* qe) {

  std::string name = qe->GetElementContent("prototype");
  int number = atoi(qe->GetElementContent("number").c_str());

  CLOG(LEV_DEBUG3) << "Institution: " << this->name() << " is adding "
                   << number << " prototypes of type " << name
                   << " to its list of initial facilities to build.";

  initial_build_order_.insert(std::make_pair(name, number));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string InstModel::str() {
  if (parent() != NULL) {
    return Model::str() + " in region" + parent()->name();
  } else {
    return Model::str() + " with no region.";
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InstModel::Deploy(Model* parent) {
  Model::Deploy(parent);
  context()->RegisterTimeListener(this);

  // build initial prototypes
  std::map<std::string, int>::iterator it;
  for (it = initial_build_order_.begin();
       it != initial_build_order_.end(); it ++) {

    // for each prototype
    std::string proto_name = it->first;
    int number = it->second;

    for (int i = 0; i < number; i++) {
      // build as many as required
      Model* m = context()->CreateModel<Model>(proto_name);
      m->Deploy(this);
    }
  }
}

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
    RegisterCloneAsDecommissioned(child);
    child->Decommission();
  }
}

/* --------------------
 * all INSTMODEL classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InstModel::BuildNotify(Model* m) {
  RegisterCloneAsBuilt(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InstModel::RegisterCloneAsBuilt(Model* clone) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InstModel::RegisterCloneAsDecommissioned(Model* clone) {}

} // namespace cyclus
