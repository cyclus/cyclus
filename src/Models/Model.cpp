// Model.cpp
// Implements the Model Class

#include "suffix.h"
#include "Model.h"

#include "CycException.h"
#include "Env.h"
#include "InputXML.h"
#include "Timer.h"
#include "BookKeeper.h"


#include DYNAMICLOADLIB
#include <iostream>
#include "Logger.h"

// Default starting ID for all Models is zero.
int Model::next_id_ = 0;
std::vector<Model*> Model::template_list_;
std::vector<Model*> Model::model_list_;
map<string, mdl_ctor*> Model::create_map_;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Model::getTemplateByName(std::string name) {
  Model* found_model = NULL;

  for (int i = 0; i < template_list_.size(); i++) {
    if (name == template_list_.at(i)->name()) {
      found_model = template_list_.at(i);
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
  CLOG(LEV_INFO1) << "There are " << model_list_.size() << " models.";
  CLOG(LEV_INFO2) << "Model list {";
  for (int i = 0; i < model_list_.size(); i++) {
    model_list_.at(i)->print();
  }
  CLOG(LEV_INFO2) << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
  mdl_ctor* model_constructor = loadConstructor(model_orig->modelType(),model_orig->modelImpl());
  
  Model* model_copy = model_constructor();
  
  model_copy->copyFreshModel(model_orig);

  return model_copy;
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_institutions() {

  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/simulation/region/institution");
  
  for (int i=0;i<nodes->nodeNr;i++) {
    create("Inst",nodes->nodeTab[i]);   
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::init(xmlNodePtr cur) {
  name_ = XMLinput->getCurNS() + XMLinput->get_xpath_content(cur,"name");
  CLOG(LEV_DEBUG1) << "Model '" << name_ << "' just created.";
  model_impl_ = XMLinput->get_xpath_name(cur, "model/*");
  this->setBornOn( TI->time() );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::copy(Model* model_orig) {
  if (model_orig->modelType() != model_type_ && 
       model_orig->modelImpl() != model_impl_) {
    throw CycTypeException("Cannot copy a model of type " 
        + model_orig->modelType() + "/" + model_orig->modelImpl()
        + " to an object of type "
        + model_type_ + "/" + model_impl_);
  }

  name_ = model_orig->name();
  model_impl_ = model_orig->modelImpl();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model::Model() {

  ID_ = next_id_++;
  is_template_ = true;
  template_list_.push_back(this);
  parent_ = NULL;
  MLOG(LEV_DEBUG3) << "Model ID=" << ID_ << ", ptr=" << this << " created.";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model::~Model() {
  diedOn_ = TI->time();

  // book-keeping
  BI->registerModelDatum<std::string>(ID_, "name", name());
  BI->registerModelDatum<std::string>(ID_, "modelImpl", modelImpl());
  BI->registerModelDatum<int>(ID_, "parentID", parentID_);
  BI->registerModelDatum<int>(ID_, "bornOn", bornOn());
  BI->registerModelDatum<int>(ID_, "diedOn", diedOn());

  // remove references to self
  MLOG(LEV_DEBUG3) << "Model " << name() << " ID=" << ID_ << " beginning delete.";
  removeFromList(this, template_list_);
  removeFromList(this, model_list_);

  if (parent_ != NULL) {
    parent_->removeChild(this);
  }

  // delete children
  while (children_.size() > 0) {
    Model* child = children_.at(0);
    MLOG(LEV_DEBUG4) << "deleting child model ID=" << child->ID();
    delete child;
  }
  MLOG(LEV_DEBUG3) << "Model ID=" << ID_ << ", ptr=" << this << "finished delete.";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::setIsTemplate(bool is_template) {
  is_template_ = is_template;

  if (!is_template) {
    // this prevents duplicates from being stored in the list
    removeFromList(this, model_list_);
    model_list_.push_back(this);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::removeFromList(Model* model, std::vector<Model*> &mlist) {
  for (int i = 0; i < mlist.size(); i++) {
    if (mlist[i] == model) {
      mlist.erase(mlist.begin() + i);
      break;
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::print() { 
  CLOG(LEV_INFO2) << model_type_ << "_" << name_ 
      << " ( "
      << "ID=" << ID_
      << ", implementation=" << model_impl_
      << ",  name=" << name_
      << ",  parentID=" << parentID_
      << " ) " ;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::setParent(Model* parent){ 
  // set the pointer to this model's parent
  if (parent == this) {
    throw CycIndexException("A model cannot be its own parent.");
  }

  setIsTemplate(false);
  parent_ = parent;
  parentID_ = parent->ID();

  // root nodes are their own parents
  // if this node is not its own parent, add it to its parent's list of children
  if (parent_ != NULL){
    parent_->addChild(this);
  }
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::addChild(Model* child){
  if (child == this || child == NULL) {
    return;
  }
  CLOG(LEV_DEBUG2) << "Model '" << this->name() << "' ID=" << this->ID() 
		  << " has added child '" << child->name() << "' ID=" 
		  << child->ID() << " to its list of children.";
  removeFromList(child, children_);
  children_.push_back(child); 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::removeChild(Model* child){
  CLOG(LEV_DEBUG2) << "Model '" << this->name() << "' ID=" << this->ID() 
		  << " has removed child '" << child->name() << "' ID=" 
		  << child->ID() << " from its list of children.";
  removeFromList(child, children_);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const std::string Model::modelImpl() {
  return model_impl_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<rsrc_ptr> Model::removeResource(msg_ptr order) {
  std::string msg = "The model " + name();
  msg += " doesn't support resource removal.";
  throw CycOverrideException(msg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::addResource(msg_ptr msg,
                            std::vector<rsrc_ptr> manifest) {
  std::string err_msg = "The model " + name();
  err_msg += " doesn't support resource receiving.";
  throw CycOverrideException(err_msg);
}
