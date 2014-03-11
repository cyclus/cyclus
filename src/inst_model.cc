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

InstModel::InstModel(Context* ctx) : Model(ctx) {
  kind_ = "Inst";
}

void InstModel::InitFrom(InstModel* m) {
  Model::InitFrom(m);
}

void InstModel::InfileToDb(QueryEngine* qe, DbInit di) {
  Model::InfileToDb(qe, di);
}

void InstModel::InitFrom(QueryBackend* b) {
  Model::InitFrom(b);
}

void InstModel::Snapshot(DbInit di) {
  Model::Snapshot(di);
}

std::string InstModel::str() {
  if (parent() != NULL) {
    return Model::str() + " in region" + parent()->prototype();
  } else {
    return Model::str() + " with no region.";
  }
}

void InstModel::Build(Model* parent) {
  Model::Build(parent);
}

void InstModel::DoRegistration() {
  context()->RegisterTimeListener(this);
}

void InstModel::Decommission() {
  context()->UnregisterTimeListener(this);
  Model::Decommission();
}

void InstModel::Tock(int time) {
  std::vector<Model*> to_decomm;
  for (int i = 0; i < children().size(); i++) {
    FacilityModel* child = dynamic_cast<FacilityModel*>(children().at(i));
    int lifetime = child->lifetime();
    if (lifetime != -1 && time >= child->enter_time() + lifetime) {
      CLOG(LEV_INFO3) << child->prototype() << " has reached the end of its lifetime";
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
