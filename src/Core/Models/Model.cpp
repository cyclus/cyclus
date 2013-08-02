// Model.cpp
// Implements the Model Class

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#include "Model.h"

#include "DynamicModule.h"
#include "Logger.h"
#include "CycException.h"
#include "Env.h"
#include "Timer.h"
#include "Resource.h"
#include "Prototype.h"
#include "QueryEngine.h"
#include "EventManager.h"

#include "RegionModel.h"

namespace cyclus {

// static members
int Model::next_id_ = 0;
std::vector<Model*> Model::model_list_;
std::map< std::string, boost::shared_ptr<DynamicModule> > Model::loaded_modules_;
std::vector<void*> Model::dynamic_libraries_;
std::set<Model*> Model::markets_;
std::set<Model*> Model::regions_;

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
Model* Model::getModelByName(std::string name) {
  Model* found_model = NULL;

  for (int i = 0; i < model_list_.size(); i++) {
    if (name == model_list_.at(i)->name()) {
      found_model = model_list_.at(i);
      break;
    }
  }

  if (found_model == NULL) {
    std::string err_msg = "Model '" + name + "' doesn't exist.";
    throw CycIndexException(err_msg);
  }
  return found_model;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::printModelList() {
  CLOG(LEV_INFO1) << "There are " << model_list_.size() << " models.";
  CLOG(LEV_INFO3) << "Model list {";
  for (int i = 0; i < model_list_.size(); i++) {
    CLOG(LEV_INFO3) << model_list_.at(i)->str();
  }
  CLOG(LEV_INFO3) << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<Model*> Model::getModelList() {
  return Model::model_list_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::loadModule(std::string model_type, std::string module_name)
{
  boost::shared_ptr<DynamicModule> 
    module(new DynamicModule(model_type,module_name)); 

  module->initialize();
  
  loaded_modules_.insert(make_pair(module_name,module));
  
  CLOG(LEV_DEBUG1) << "Module '" << module_name
                   << "' of type: " << model_type 
                   << " has been loaded.";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::unloadModules()
{
  std::map<std::string,boost::shared_ptr<DynamicModule> >::iterator it;
  for (it = loaded_modules_.begin(); it != loaded_modules_.end(); it++)
    {
      it->second->closeLibrary();
    }
  loaded_modules_.erase(loaded_modules_.begin(),loaded_modules_.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::initializeSimulationEntity(std::string model_type, 
                                       QueryEngine* qe) {
  // query data
  QueryEngine* module_data = qe->queryElement("model");
  std::string module_name = module_data->getElementName();

  // instantiate & init module
  /* --- */
  loadModule(model_type, module_name);
  Model* model = constructModel(module_name);
  /* --- */
  model->initCoreMembers(qe);
  model->setModelImpl(module_name);
  model->initModuleMembers(module_data->queryElement(module_name));

  CLOG(LEV_DEBUG3) << "Module '" << model->name()
                   << "' has had its module members initialized:";
  CLOG(LEV_DEBUG3) << " * Type: " << model->modelType(); 
  CLOG(LEV_DEBUG3) << " * Implementation: " << model->modelImpl() ;
  CLOG(LEV_DEBUG3) << " * ID: " << model->ID();

  // register module
  if ("Facility" == model_type) {
    Prototype::registerPrototype(model->name(),
        			 dynamic_cast<Prototype*>(model));
  } else if ("Market" == model_type) {
    registerMarketWithSimulation(model);
  }  else {
    model_list_.push_back(model);
  }
  if ("Region" == model_type) {
    registerRegionWithSimulation(model);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::registerMarketWithSimulation(Model* market) {
  MarketModel* marketCast = dynamic_cast<MarketModel*>(market);
  if (!marketCast) {
    std::string err_msg = "Model '" + market->name() + "' can't be registered as a market.";
    throw CycOverrideException(err_msg);
  }
  else {
    markets_.insert(market);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::registerRegionWithSimulation(Model* region) {
  RegionModel* regionCast = dynamic_cast<RegionModel*>(region);
  if (!regionCast) {
    std::string err_msg = "Model '" + region->name() + "' can't be registered as a region.";
    throw CycOverrideException(err_msg);
  }
  else {
    regions_.insert(region);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::constructSimulation() {
  std::set<Model*>::iterator it;
  for (it = markets_.begin(); it != markets_.end(); it++) {
    Model* market = *it;
    market->enterSimulation(market);
  }
  for (it = regions_.begin(); it != regions_.end(); it++) {
    Model* region = *it;
    region->enterSimulation(region);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::initCoreMembers(QueryEngine* qe) {
  name_ = qe->getElementContent("name");
  CLOG(LEV_DEBUG1) << "Model '" << name_ << "' just created.";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model::Model() {
  children_ = std::vector<Model*>();
  name_ = "";
  ID_ = next_id_++;
  born_ = false;
  parent_ = NULL;
  parentID_ = -1;
  MLOG(LEV_DEBUG3) << "Model ID=" << ID_ << ", ptr=" << this << " created.";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model::~Model() {
  MLOG(LEV_DEBUG3) << "Deleting model '" << name() << "' ID=" << ID_ << " {";
  
  // set died on date and record it in the table
  diedOn_ = TI->time();

  EM->newEvent("AgentDeaths")
    ->addVal("AgentID", ID())
    ->addVal("DeathDate", diedOn_)
    ->record();
  
  // remove references to self
  removeFromList(this, model_list_);

  if (parent_ != NULL) {
    parent_->removeChild(this);
  }

  // delete children
  while (children_.size() > 0) {
    Model* child = children_.at(0);
    MLOG(LEV_DEBUG4) << "Deleting child model ID=" << child->ID() << " {";
    deleteModel(child);
    MLOG(LEV_DEBUG4) << "}";
  }
  MLOG(LEV_DEBUG3) << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Model::constructModel(std::string model_impl){
  if (loaded_modules_.find(model_impl) == loaded_modules_.end())
    throw CycOverrideException("No module is registered for " + model_impl);

  return loaded_modules_[model_impl]->constructInstance();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::deleteModel(Model* model){
  std::map<std::string, boost::shared_ptr<DynamicModule> >::iterator it;
  it = loaded_modules_.find(model->modelImpl());
  if (it != loaded_modules_.end()) 
    it->second->destructInstance(model);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::removeFromList(Model* model, std::vector<Model*> &mlist) {
  std::vector<Model*>::iterator it = find(mlist.begin(),mlist.end(),model);
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
void Model::enterSimulation(Model* parent){ 

  CLOG(LEV_DEBUG1) << "Model '" << name()
                   << "' is entering the simulation.";
  CLOG(LEV_DEBUG3) << "It has:";
  CLOG(LEV_DEBUG3) << " * Implementation: " << modelImpl();
  CLOG(LEV_DEBUG3) << " * ID: " << ID();

  // set model-specific members
  parentID_ = parent->ID();
  setParent(parent);
  if (parent != this)
    parent->addChild(this);
  bornOn_ = TI->time();

  // add model to the database
  this->addToTable();

  enterSimulationAsCoreEntity();
  enterSimulationAsModule();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::enterSimulationAsCoreEntity() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::enterSimulationAsModule() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::setParent(Model* parent){ 
  if (parent == this) {
    // root nodes are their own parent
    parent_ = NULL; // parent pointer set to NULL for memory management
  } else {
    parent_ = parent;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Model::parent(){
  // if parent pointer is null, throw an error
  if (parent_ == NULL){
    std::string null_err = "You have tried to access the parent of " +	\
      this->name() + " but the parent pointer is NULL.";
    throw CycIndexException(null_err);
  }
  // else return pointer to parent
  return parent_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::addChild(Model* child){
  if (child == this)
    throw CycOverrideException("Model " + name() + 
                               "is trying to add itself as its own child.");
      
  if (!child)
    throw CycOverrideException("Model " + name() + 
                               "is trying to add an invalid model as its child.");
    

  CLOG(LEV_DEBUG2) << "Model '" << name() << "' ID=" << ID() 
		  << " has added child '" << child->name() << "' ID=" 
		  << child->ID() << " to its list of children.";

  children_.push_back(child); 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::removeChild(Model* child){
  CLOG(LEV_DEBUG2) << "Model '" << this->name() << "' ID=" << this->ID() 
		  << " has removed child '" << child->name() << "' ID=" 
		  << child->ID() << " from its list of children.";
  removeFromList(child, children_);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Model::printChildren() {
  std::stringstream ss("");
  ss << "Children of " << name() << ":" << std::endl;
  for (int i = 0; i < children_.size(); i++) {
    std::vector<std::string> print_outs = getTreePrintOuts(children_.at(i));
    for (int j = 0; j < print_outs.size(); j++) {
      ss << "\t" << print_outs.at(j);
    }
  }
  return ss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<std::string> Model::getTreePrintOuts(Model* m) {
  std::vector<std::string> ret;
  std::stringstream ss("");
  ss << m->name() << std::endl;
  ret.push_back(ss.str());
  for (int i = 0; i < m->nChildren(); i++) {
    std::vector<std::string> outs = getTreePrintOuts(m->children(i));
    for (int j = 0; j < outs.size(); j++) {
      ss.str("");
      ss << "\t" << outs.at(j) << std::endl;
      ret.push_back(ss.str());
    }
  }
  return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const std::string Model::modelImpl() {
  return model_impl_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<rsrc_ptr> Model::removeResource(Transaction order) {
  std::string msg = "The model " + name();
  msg += " doesn't support resource removal.";
  throw CycOverrideException(msg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::addResource(Transaction trans,
			std::vector<rsrc_ptr> manifest) {
  std::string err_msg = "The model " + name();
  err_msg += " doesn't support resource receiving.";
  throw CycOverrideException(err_msg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::addToTable(){
  EM->newEvent("Agents")
    ->addVal("ID", ID())
    ->addVal("AgentType", modelType())
    ->addVal("ModelType", modelImpl())
    ->addVal("Prototype", name())
    ->addVal("ParentID", parentID())
    ->addVal("EnterDate", bornOn())
    ->record();
}
} // namespace cyclus
