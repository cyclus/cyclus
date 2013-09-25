// model.cc
// Implements the Model Class
#include "model.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

#include "error.h"
#include "logger.h"
#include "market_model.h"
#include "region_model.h"
#include "resource.h"

namespace cyclus {

// static members
int Model::next_id_ = 0;
std::vector<Model*> Model::model_list_;
std::map< std::string, DynamicModule*>
Model::loaded_modules_;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::set<std::string> Model::dynamic_module_types() {
  std::set<std::string> types;
  types.insert("Market");
  types.insert("Converter");
  types.insert("Region");
  types.insert("Inst");
  types.insert("Facility");
  return types;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Model::GetModelByName(std::string name) {
  Model* found_model = NULL;

  for (int i = 0; i < model_list_.size(); i++) {
    if (name == model_list_.at(i)->name()) {
      found_model = model_list_.at(i);
      break;
    }
  }

  if (found_model == NULL) {
    std::string err_msg = "Model '" + name + "' doesn't exist.";
    throw KeyError(err_msg);
  }
  return found_model;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::PrintModelList() {
  CLOG(LEV_INFO1) << "There are " << model_list_.size() << " models.";
  CLOG(LEV_INFO3) << "Model list {";
  for (int i = 0; i < model_list_.size(); i++) {
    CLOG(LEV_INFO3) << model_list_.at(i)->str();
  }
  CLOG(LEV_INFO3) << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<Model*> Model::GetModelList() {
  return Model::model_list_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::UnloadModules() {
  std::map<std::string, DynamicModule*>::iterator it;
  for (it = loaded_modules_.begin(); it != loaded_modules_.end(); it++) {
    it->second->CloseLibrary();
    delete it->second;
  }
  loaded_modules_.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::InitFrom(Model* m) {
  ID_ = next_id_++;
  name_ = m->name_;
  model_type_ = m->model_type_;
  model_impl_ = m->model_impl_;
  model_type_ = m->model_type_;
  ctx_ = m->ctx_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::InitializeSimulationEntity(Context* ctx, std::string model_type,
                                       QueryEngine* qe) {
  // query data
  QueryEngine* module_data = qe->QueryElement("model");
  std::string module_name = module_data->GetElementName();

  DynamicModule* module = new DynamicModule(model_type, module_name);
  loaded_modules_[module_name] = module;
  CLOG(LEV_DEBUG1) << "Module '" << module_name
                   << "' of type: " << model_type
                   << " has been loaded.";
  
  Model* model = module->ConstructInstance(ctx);
  model->InitCoreMembers(qe);
  model->SetModelImpl(module_name);
  model->InitModuleMembers(module_data->QueryElement(module_name));

  CLOG(LEV_DEBUG3) << "Module '" << model->name()
                   << "' has had its module members initialized:";
  CLOG(LEV_DEBUG3) << " * Type: " << model->ModelType();
  CLOG(LEV_DEBUG3) << " * Implementation: " << model->ModelImpl() ;
  CLOG(LEV_DEBUG3) << " * ID: " << model->ID();

  // register module
  if ("Facility" == model_type) {
    ctx->AddPrototype(model->name(), model);
  } else if (model_type == "Market" || model_type == "Region") {
    model->Deploy(model);
  }
  model_list_.push_back(model);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::InitCoreMembers(QueryEngine* qe) {
  name_ = qe->GetElementContent("name");
  CLOG(LEV_DEBUG1) << "Model '" << name_ << "' just created.";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model::Model(Context* ctx)
  : ctx_(ctx),
    ID_(next_id_++),
    parentID_(-1),
    birthtime_(-1),
    deathtime_(-1),
    parent_(NULL) {
  MLOG(LEV_DEBUG3) << "Model ID=" << ID_ << ", ptr=" << this << " created.";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model::~Model() {
  MLOG(LEV_DEBUG3) << "Deleting model '" << name() << "' ID=" << ID_ << " {";
  // set died on date and record it in the table if it was ever deployed
  if (birthtime_ > -1) {
    deathtime_ = ctx_->time();
    ctx_->NewEvent("AgentDeaths")
    ->AddVal("AgentID", ID())
    ->AddVal("DeathDate", deathtime_)
    ->Record();
  }

  // remove references to self
  RemoveFromList(this, model_list_);

  if (parent_ != NULL) {
    parent_->RemoveChild(this);
  }

  // delete children
  while (children_.size() > 0) {
    Model* child = children_.at(0);
    MLOG(LEV_DEBUG4) << "Deleting child model ID=" << child->ID() << " {";
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
     << "ID=" << ID_
     << ", implementation=" << model_impl_
     << ",  name=" << name_
     << ",  parentID=" << parentID_
     << " ) " ;
  return ss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::Deploy(Model* parent) {

  CLOG(LEV_DEBUG1) << "Model '" << name()
                   << "' is entering the simulation.";
  CLOG(LEV_DEBUG3) << "It has:";
  CLOG(LEV_DEBUG3) << " * Implementation: " << ModelImpl();
  CLOG(LEV_DEBUG3) << " * ID: " << ID();

  // set model-specific members
  parentID_ = parent->ID();
  SetParent(parent);
  if (parent != this) {
    parent->AddChild(this);
  }
  birthtime_ = ctx_->time();

  // add model to the database
  this->AddToTable();
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
Model* Model::parent() {
  // if parent pointer is null, throw an error
  if (parent_ == NULL) {
    std::string null_err = "You have tried to access the parent of " +  \
                           this->name() + " but the parent pointer is NULL.";
    throw ValueError(null_err);
  }
  // else return pointer to parent
  return parent_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::AddChild(Model* child) {
  if (child == this)
    throw KeyError("Model " + name() +
                   "is trying to add itself as its own child.");

  if (!child)
    throw ValueError("Model " + name() +
                     "is trying to add an invalid model as its child.");


  CLOG(LEV_DEBUG2) << "Model '" << name() << "' ID=" << ID()
                   << " has added child '" << child->name() << "' ID="
                   << child->ID() << " to its list of children.";

  children_.push_back(child);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::RemoveChild(Model* child) {
  CLOG(LEV_DEBUG2) << "Model '" << this->name() << "' ID=" << this->ID()
                   << " has removed child '" << child->name() << "' ID="
                   << child->ID() << " from its list of children.";
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
  for (int i = 0; i < m->NChildren(); i++) {
    std::vector<std::string> outs = GetTreePrintOuts(m->children(i));
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<Resource::Ptr> Model::RemoveResource(Transaction order) {
  std::string msg = "The model " + name();
  msg += " doesn't support resource removal.";
  throw Error(msg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::AddResource(Transaction trans,
                        std::vector<Resource::Ptr> manifest) {
  std::string err_msg = "The model " + name();
  err_msg += " doesn't support resource receiving.";
  throw Error(err_msg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::AddToTable() {
  ctx_->NewEvent("Agents")
  ->AddVal("ID", ID())
  ->AddVal("AgentType", ModelType())
  ->AddVal("ModelType", ModelImpl())
  ->AddVal("Prototype", name())
  ->AddVal("ParentID", ParentID())
  ->AddVal("EnterDate", birthtime())
  ->Record();
}
} // namespace cyclus
