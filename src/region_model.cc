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

void RegionModel::InitFrom(RegionModel* m) {
  Model::InitFrom(m);
}

RegionModel::RegionModel(Context* ctx) : Model(ctx) {
  kind_ = "Region";
}

void RegionModel::InfileToDb(QueryEngine* qe, DbInit di) {
  Model::InfileToDb(qe, di);
}

void RegionModel::Snapshot(DbInit di) {
  Model::Snapshot(di);
}

void RegionModel::Build(Model* parent) {
  Model::Build(parent);
}

void RegionModel::DoRegistration() {
  context()->RegisterTimeListener(this);
}

void RegionModel::Decommission() {
  context()->UnregisterTimeListener(this);
  Model::Decommission();
}

std::string RegionModel::str() {
  std::string s = Model::str();

  s += " has insts: ";
  for (std::vector<Model*>::const_iterator inst = children().begin();
       inst != children().end();
       inst++) {
    s += (*inst)->prototype() + ", ";
  }
  return s;
}

} // namespace cyclus
