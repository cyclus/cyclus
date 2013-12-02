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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RegionModel::RegionModel(Context* ctx) : TimeAgent(ctx), Model(ctx) {
  SetModelType("Region");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RegionModel::InitCoreMembers(QueryEngine* qe) {
  Model::InitCoreMembers(qe); // name_
  RegionModel::InitAllowedFacilities(qe); // allowedFacilities_
  RegionModel::InitInstitutionNames(qe); // inst_names_
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
void RegionModel::InitInstitutionNames(QueryEngine* qe) {
  int nInsts = qe->NElementsMatchingQuery("institution");
  std::string name;
  for (int i = 0; i < nInsts; i++) {
    QueryEngine* inst_data = qe->QueryElement("institution", i);
    name = inst_data->GetElementContent("name");
    inst_names_.insert(name);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RegionModel::Deploy(Model* parent) {
  Model::Deploy(parent);
  AddRegionAsRootNode();
  AddChildrenToTree();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RegionModel::AddRegionAsRootNode() {
  context()->RegisterTicker(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RegionModel::AddChildrenToTree() {
  Model* inst;
  std::set<std::string>::iterator it;
  for (it = inst_names_.begin(); it != inst_names_.end(); it++) {
    inst = context()->GetModelByName((*it));
    inst->Deploy(this);
  }
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
    s += (*inst)->name() + ", ";
  }
  return s;
}

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RegionModel::ReceiveMessage(Message::Ptr msg) {
  msg->SendOn();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RegionModel::HandleTick(int time) {
  int currsize = children().size();
  int i = 0;
  while (i < children().size()) {
    Model* m = children().at(i);
    dynamic_cast<InstModel*>(m)->HandleTick(time);

    // increment not needed if a facility deleted itself
    if (children().size() == currsize) {
      i++;
    }
    currsize = children().size();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RegionModel::HandleTock(int time) {
  int currsize = children().size();
  int i = 0;
  while (i < children().size()) {
    Model* m = children().at(i);
    dynamic_cast<InstModel*>(m)->HandleTock(time);

    // increment not needed if a facility deleted itself
    if (children().size() == currsize) {
      i++;
    }
    currsize = children().size();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RegionModel::HandleDailyTasks(int time, int day) {
  // tell all of the institution models to handle the tick
  for (std::vector<Model*>::const_iterator inst = children().begin();
       inst != children().end();
       inst++) {
    (dynamic_cast<InstModel*>(*inst))->HandleDailyTasks(time, day);
  }
}
} // namespace cyclus
