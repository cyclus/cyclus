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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::InitFrom(Model* m) {
  id_ = next_id_++;
  name_ = m->name_;
  model_type_ = m->model_type_;
  model_impl_ = m->model_impl_;
  ctx_ = m->ctx_;
  ctx_->model_list_.insert(this); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Model::InformErrorMsg(std::string msg) {
  std::stringstream ret;
  ret << "A(n) " << model_impl_ << " named " << name_
      << " at time " << context()->time()
      << " received the following error:\n"
      << msg;
  return ret.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::InitFrom(QueryEngine* qe) {
  name_ = qe->GetElementContent("name");
  CLOG(LEV_DEBUG1) << "Model '" << name_ << "' just created.";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model::Model(Context* ctx)
  : ctx_(ctx),
    id_(next_id_++),
    model_type_("Model"),
    parent_id_(-1),
    birthtime_(-1),
    deathtime_(-1),
    parent_(NULL),
    model_impl_("UNSPECIFIED") {
  ctx_->model_list_.insert(this); 
  MLOG(LEV_DEBUG3) << "Model ID=" << id_ << ", ptr=" << this << " created.";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model::~Model() {
  MLOG(LEV_DEBUG3) << "Deleting model '" << name() << "' ID=" << id_ << " {";
  context()->model_list_.erase(this);
  // set died on date and record it in the table if it was ever built
  if (birthtime_ > -1) {
    deathtime_ = ctx_->time();
    ctx_->NewDatum("AgentExit")
    ->AddVal("AgentId", id())
    ->AddVal("ExitTime", deathtime_)
    ->Record();
  }
  
  if (parent_ != NULL) {
    CLOG(LEV_DEBUG2) << "Model '" << parent_->name() << "' ID=" << parent_->id()
                     << " has removed child '" << name() << "' ID="
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Model::str() {
  std::stringstream ss;
  ss << model_type() << "_" << name_
     << " ( "
     << "ID=" << id_
     << ", implementation=" << model_impl_
     << ",  name=" << name_
     << ",  parentID=" << parent_id_
     << " ) " ;
  return ss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::Build(Model* parent) {
  if (parent == this)
    throw KeyError("Model " + name() +
                   "is trying to add itself as its own child.");

  CLOG(LEV_DEBUG1) << "Model '" << name()
                   << "' is entering the simulation.";
  CLOG(LEV_DEBUG3) << "It has:";
  CLOG(LEV_DEBUG3) << " * Implementation: " << ModelImpl();
  CLOG(LEV_DEBUG3) << " * ID: " << id();

  if (parent != NULL) {
    parent_ = parent;
    parent_id_ = parent->id();
    parent->children_.push_back(this);
    CLOG(LEV_DEBUG2) << "Model '" << parent->name() << "' ID=" << parent->id()
                     << " has added child '" << name() << "' ID="
                     << id() << " to its list of children.";
  }
  birthtime_ = ctx_->time();
  this->AddToTable();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::Decommission() {
  CLOG(LEV_INFO3) << name() << " is being decommissioned";
  ctx_->DelModel(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Model::PrintChildren() {
  std::stringstream ss("");
  ss << "Children of " << name() << ":" << std::endl;
  for (int i = 0; i < children_.size(); i++) {
    std::vector<std::string> print_outs = GetTreePrintOuts(children_.at(i));
    for (int j = 0; j < print_outs.size(); j++) {
      ss << "\t" << print_outs.at(j);
    }
  }
  return ss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<std::string> Model::GetTreePrintOuts(Model* m) {
  std::vector<std::string> ret;
  std::stringstream ss("");
  ss << m->name() << std::endl;
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const std::string Model::ModelImpl() {
  return model_impl_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::AddToTable() {
  ctx_->NewDatum("AgentEntry")
  ->AddVal("AgentId", id())
  ->AddVal("Kind", model_type())
  ->AddVal("Implementation", ModelImpl())
  ->AddVal("Prototype", name())
  ->AddVal("ParentId", parent_id_)
  ->AddVal("EnterTime", birthtime())
  ->Record();
}
} // namespace cyclus
