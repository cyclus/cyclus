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
std::set<std::string> Model::dynamic_module_types() {
  std::set<std::string> types;
  types.insert("Region");
  types.insert("Inst");
  types.insert("Facility");
  return types;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::InitFrom(Model* m) {
  id_ = next_id_++;
  name_ = m->name_;
  model_type_ = m->model_type_;
  model_impl_ = m->model_impl_;
  ctx_ = m->ctx_;
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
void Model::InitCoreMembers(QueryEngine* qe) {
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
    parent_(NULL) {
  MLOG(LEV_DEBUG3) << "Model ID=" << id_ << ", ptr=" << this << " created.";
  ctx_->AddModel(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model::~Model() {
  MLOG(LEV_DEBUG3) << "Deleting model '" << name() << "' ID=" << id_ << " {";
  // set died on date and record it in the table if it was ever deployed
  if (birthtime_ > -1) {
    deathtime_ = ctx_->time();
    ctx_->NewDatum("AgentDeaths")
    ->AddVal("AgentID", id())
    ->AddVal("DeathDate", deathtime_)
    ->Record();
  }
  
  // remove references to self
  RemoveFromList(this, ctx_->model_list());

  if (parent_ != NULL) {
    parent_->RemoveChild(this);
  }

  // delete children
  while (children_.size() > 0) {
    Model* child = children_.at(0);
    MLOG(LEV_DEBUG4) << "Deleting child model ID=" << child->id() << " {";
    delete child;
    MLOG(LEV_DEBUG4) << "}";
  }
  MLOG(LEV_DEBUG3) << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::RemoveFromList(Model* model, std::vector<Model*>& mlist) {
  std::vector<Model*>::iterator it = find(mlist.begin(), mlist.end(), model);
  if (it != mlist.end()) {
    mlist.erase(it);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Model::str() {
  std::stringstream ss;
  ss << model_type_ << "_" << name_
     << " ( "
     << "ID=" << id_
     << ", implementation=" << model_impl_
     << ",  name=" << name_
     << ",  parentID=" << parent_id_
     << " ) " ;
  return ss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::Deploy(Model* parent) {
  CLOG(LEV_DEBUG1) << "Model '" << name()
                   << "' is entering the simulation.";
  CLOG(LEV_DEBUG3) << "It has:";
  CLOG(LEV_DEBUG3) << " * Implementation: " << ModelImpl();
  CLOG(LEV_DEBUG3) << " * ID: " << id();

  if (parent == NULL) parent = this;
  
  // set model-specific members
  parent_id_ = parent->id();
  SetParent(parent);
  if (parent != this) {
    parent->AddChild(this);
  }
  birthtime_ = ctx_->time();

  // add model to the database
  this->AddToTable();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::Decommission() {
  CLOG(LEV_INFO3) << name() << " is being decommissioned";
  delete this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::SetParent(Model* parent) {
  if (parent == this) {
    // root nodes are their own parent
    parent_ = NULL; // parent pointer set to NULL for memory management
  } else {
    parent_ = parent;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::AddChild(Model* child) {
  if (child == this)
    throw KeyError("Model " + name() +
                   "is trying to add itself as its own child.");

  if (!child)
    throw ValueError("Model " + name() +
                     "is trying to add an invalid model as its child.");


  CLOG(LEV_DEBUG2) << "Model '" << name() << "' ID=" << id()
                   << " has added child '" << child->name() << "' ID="
                   << child->id() << " to its list of children.";

  children_.push_back(child);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::RemoveChild(Model* child) {
  CLOG(LEV_DEBUG2) << "Model '" << this->name() << "' ID=" << this->id()
                   << " has removed child '" << child->name() << "' ID="
                   << child->id() << " from its list of children.";
  RemoveFromList(child, children_);
};

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
  ctx_->NewDatum("Agents")
  ->AddVal("ID", id())
  ->AddVal("AgentType", ModelType())
  ->AddVal("ModelType", ModelImpl())
  ->AddVal("Prototype", name())
  ->AddVal("ParentID", parent_id())
  ->AddVal("EnterDate", birthtime())
  ->Record();
}
} // namespace cyclus
