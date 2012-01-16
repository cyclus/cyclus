// Model.cpp
// Implements the Model Class

#include "suffix.h"
#include "Model.h"

#include "CycException.h"
#include "Env.h"
#include "InputXML.h"
#include "Timer.h"

#include "RegionModel.h"

#include DYNAMICLOADLIB
#include <iostream>
#include "Logger.h"

// Default starting ID for all Models is zero.
int Model::next_id_ = 0;
std::vector<Model*> Model::model_list_;
map<string, mdl_ctor*> Model::create_map_;
map<string, mdl_dtor*> Model::destroy_map_;

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
    string err_msg = "Model '" + name + "' doesn't exist.";
    throw CycIndexException(err_msg);
  }
  return found_model;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::printModelList() {
  for (int i = 0; i < model_list_.size(); i++) {
    model_list_.at(i)->print();
  }
}

std::vector<Model*> Model::getModelList() {
  return Model::model_list_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::create(std::string model_type, xmlNodePtr cur) {
  string model_impl = XMLinput->get_xpath_name(cur, "model/*");

  // get instance
  mdl_ctor* model_constructor = loadConstructor(model_type, model_impl);

  Model* model = model_constructor();

  model->init(cur);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Model::create(Model* model_orig) {
  mdl_ctor* model_constructor = loadConstructor(model_orig->getModelType(),model_orig->getModelImpl());
  
  Model* model_copy = model_constructor();
  
  model_copy->copyFreshModel(model_orig);

  return model_copy;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void* Model::destroy(Model* model) {
  mdl_dtor* model_destructor = destroy_map_[model->getModelImpl()];

  model_destructor(model);
  
  return model;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Model::decommission(){  
  // set the died on date for book keeping
  this->setDiedOn( TI->getTime() );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_models() {
  load_converters();
  load_markets();
  load_facilities();
  load_regions();
  load_institutions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_markets() {
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/*/market");
  
  for (int i=0;i<nodes->nodeNr;i++) {
    create("Market",nodes->nodeTab[i]);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_converters() {

  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/*/converter");
  
  for (int i=0;i<nodes->nodeNr;i++) {
    create("Converter",nodes->nodeTab[i]);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_facilities() {
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/*/facilitycatalog");
  
  for (int i=0;i<nodes->nodeNr;i++){
    load_facilitycatalog(XMLinput->get_xpath_content(nodes->nodeTab[i], 
                         "filename"),
        XMLinput->get_xpath_content(nodes->nodeTab[i], "namespace"),
        XMLinput->get_xpath_content(nodes->nodeTab[i], "format"));
  }

  nodes = XMLinput->get_xpath_elements("/*/facility");
  
  for (int i=0;i<nodes->nodeNr;i++) {
    create("Facility",nodes->nodeTab[i]);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_facilitycatalog(std::string filename, std::string ns, std::string format){
  XMLinput->extendCurNS(ns);

  if ("xml" == format){
    XMLinput->load_facilitycatalog(filename);
  } else {
    throw CycRangeException(format + "is not a supported facilitycatalog format.");
  }

  XMLinput->stripCurNS();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_regions() {

  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/simulation/region");
  
  for (int i=0;i<nodes->nodeNr;i++) {
    create("Region",nodes->nodeTab[i]);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Model::load_institutions() {

  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/simulation/region/institution");
  
  for (int i=0;i<nodes->nodeNr;i++) {
    create("Inst",nodes->nodeTab[i]);   
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::init(xmlNodePtr cur) {
  name_ = XMLinput->getCurNS() + XMLinput->get_xpath_content(cur,"name");
  LOG(LEV_DEBUG2) << "Model '" << name_ << "' just created.";
  model_impl_ = XMLinput->get_xpath_name(cur, "model/*");
  this->setBornOn( TI->getTime() );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::copy(Model* model_orig) {
  if (model_orig->getModelType() != model_type_ && 
       model_orig->getModelImpl() != model_impl_) {
    throw CycTypeException("Cannot copy a model of type " 
        + model_orig->getModelType() + "/" + model_orig->getModelImpl()
        + " to an object of type "
        + model_type_ + "/" + model_impl_);
  }

  name_ = model_orig->name();
  model_impl_ = model_orig->getModelImpl();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model::Model() {
  is_template_ = true;
  ID_ = ++next_id_;
  model_list_.push_back(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model::~Model() {
  for (int i = 0; i < model_list_.size(); i++) {
    if (model_list_[i] == this) {
      model_list_.erase(model_list_.begin() + i);
      break;
    }
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::print() { 
  LOG(LEV_DEBUG2) << model_type_ << " " << name_ 
      << " (ID=" << ID_
      << ", implementation = " << model_impl_
      << "  name = " << name_
      << " ) " ;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::setParent(Model* parent){ 
  // set the pointer to this model's parent
  parent_ = parent;
  // root nodes are their own parents
  // if this node is not its own parent, add it to its parent's list of children
  if (parent_ != this){
    parent_->addChild(this);
  }
  parentID_ = parent_->ID();
};

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::addChild(Model* child){
  LOG(LEV_DEBUG3) << "Model " << this->name() << " ID " << this->ID() 
		  << " has added child " << child->name() << " ID " 
		  << child->ID() << " to its list of children.";
  children_.push_back(child); 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const std::string Model::getModelImpl() {
  return model_impl_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<Resource*> Model::removeResource(Message* order) {
  std::string msg = "The model " + name();
  msg += " doesn't support resource transfer.";
  throw CycOverrideException(msg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::addResource(Transaction trans,
                            std::vector<Resource*> manifest) {
  std::string msg = "The model " + name();
  msg += " doesn't support resource transfer.";
  throw CycOverrideException(msg);
}
