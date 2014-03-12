// model.cc
// Implements the Model Class
#include "model.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

#include "error.h"
#include "logger.h"
#include "region_model.h"
#include "resource.h"

namespace cyclus {

// static members
int Model::next_id_ = 0;

void Model::InitFrom(Model* m) {
  id_ = next_id_++;
  prototype_ = m->prototype_;
  kind_ = m->kind_;
  model_impl_ = m->model_impl_;
  lifetime_ = m->lifetime_;
  ctx_ = m->ctx_;
}

std::string Model::InformErrorMsg(std::string msg) {
  std::stringstream ret;
  ret << "A(n) " << model_impl_ << " named " << prototype_
      << " at time " << context()->time()
      << " received the following error:\n"
      << msg;
  return ret.str();
}

void Model::InfileToDb(QueryEngine* qe, DbInit di) {
  std::string proto = qe->GetString("name");
  int lifetime = GetOptionalQuery<int>(qe, "lifetime", -1);
  di.NewDatum("Model")
    ->AddVal("Prototype", proto)
    ->AddVal("Lifetime", lifetime)
    ->Record();
}

void Model::InitFrom(QueryBackend* b) {
  QueryResult qr = b->Query("Model", NULL);
  prototype_ = qr.GetVal<std::string>("Prototype");
  lifetime_ = qr.GetVal<int>("Lifetime");
}

void Model::Snapshot(DbInit di) {
  di.NewDatum("Model")
    ->AddVal("Prototype", prototype_)
    ->AddVal("Lifetime", lifetime_)
    ->Record();
}

Model::Model(Context* ctx)
  : ctx_(ctx),
    id_(next_id_++),
    kind_("Model"),
    parent_id_(-1),
    enter_time_(-1),
    lifetime_(-1),
    parent_(NULL),
    model_impl_("UNSPECIFIED") {
  ctx_->model_list_.insert(this); 
  MLOG(LEV_DEBUG3) << "Model ID=" << id_ << ", ptr=" << this << " created.";
}

Model::~Model() {
  MLOG(LEV_DEBUG3) << "Deleting model '" << prototype() << "' ID=" << id_ << " {";
  context()->model_list_.erase(this);
  
  if (parent_ != NULL) {
    CLOG(LEV_DEBUG2) << "Model '" << parent_->prototype() << "' ID=" << parent_->id()
                     << " has removed child '" << prototype() << "' ID="
                     << id() << " from its list of children.";
    std::vector<Model*>::iterator it;
    it = find(parent_->children_.begin(), parent_->children_.end(), this);
    if (it != parent_->children_.end()) {
      parent_->children_.erase(it);
    }
  }

  // delete children
  while (children_.size() > 0) {
    Model* child = children_.at(0);
    MLOG(LEV_DEBUG4) << "Deleting child model ID=" << child->id() << " {";
    ctx_->DelModel(child);
    MLOG(LEV_DEBUG4) << "}";
  }
  MLOG(LEV_DEBUG3) << "}";
}

std::string Model::str() {
  std::stringstream ss;
  ss << kind_ << "_" << prototype_
     << " ( "
     << "ID=" << id_
     << ", implementation=" << model_impl_
     << ",  name=" << prototype_
     << ",  parentID=" << parent_id_
     << " ) " ;
  return ss.str();
}

void Model::Build(Model* parent) {
  CLOG(LEV_DEBUG1) << "Model '" << prototype()
                   << "' is entering the simulation.";
  CLOG(LEV_DEBUG3) << "It has:";
  CLOG(LEV_DEBUG3) << " * Implementation: " << model_impl_;
  CLOG(LEV_DEBUG3) << " * ID: " << id();

  Connect(parent);
  enter_time_ = ctx_->time();
  DoRegistration();
  this->AddToTable();
}

void Model::Connect(Model* parent) {
  if (parent == this) {
    throw KeyError("Model " + prototype() +
                   "is trying to add itself as its own child.");
  }
  if (parent != NULL) {
    parent_ = parent;
    parent_id_ = parent->id();
    parent->children_.push_back(this);
  }
}

void Model::Decommission() {
  CLOG(LEV_INFO3) << prototype() << " is being decommissioned";
  ctx_->NewDatum("AgentExit")
  ->AddVal("AgentId", id())
  ->AddVal("ExitTime", ctx_->time())
  ->Record();
  ctx_->DelModel(this);
}

std::string Model::PrintChildren() {
  std::stringstream ss("");
  ss << "Children of " << prototype() << ":" << std::endl;
  for (int i = 0; i < children_.size(); i++) {
    std::vector<std::string> print_outs = GetTreePrintOuts(children_.at(i));
    for (int j = 0; j < print_outs.size(); j++) {
      ss << "\t" << print_outs.at(j);
    }
  }
  return ss.str();
}

std::vector<std::string> Model::GetTreePrintOuts(Model* m) {
  std::vector<std::string> ret;
  std::stringstream ss("");
  ss << m->prototype() << std::endl;
  ret.push_back(ss.str());
  for (int i = 0; i < m->children().size(); i++) {
    std::vector<std::string> outs = GetTreePrintOuts(m->children().at(i));
    for (int j = 0; j < outs.size(); j++) {
      ss.str("");
      ss << "\t" << outs.at(j) << std::endl;
      ret.push_back(ss.str());
    }
  }
  return ret;
}

void Model::AddToTable() {
  ctx_->NewDatum("AgentEntry")
  ->AddVal("AgentId", id_)
  ->AddVal("Kind", kind_)
  ->AddVal("Implementation", model_impl_)
  ->AddVal("Prototype", prototype_)
  ->AddVal("Lifetime", lifetime_)
  ->AddVal("ParentId", parent_id_)
  ->AddVal("EnterTime", enter_time_)
  ->Record();
}
} // namespace cyclus
