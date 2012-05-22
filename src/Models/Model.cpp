// Model.cpp
// Implements the Model Class

#include <iostream>
#include <sstream>
#include <string>

#include "Model.h"

#include "Logger.h"
#include "suffix.h"
#include "CycException.h"
#include "Env.h"
#include "InputXML.h"
#include "Timer.h"
#include "Resource.h"
#include "Table.h"

#include DYNAMICLOADLIB

using namespace std;

// Default starting ID for all Models is zero.
int Model::next_id_ = 0;
// Database table for agents
table_ptr Model::agent_table = new Table("Agents"); 
// Model containers
vector<Model*> Model::template_list_;
vector<Model*> Model::model_list_;
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
  CLOG(LEV_INFO3) << "Model list {";
  for (int i = 0; i < model_list_.size(); i++) {
    CLOG(LEV_INFO3) << model_list_.at(i)->str();
  }
  CLOG(LEV_INFO3) << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
vector<Model*> Model::getModelList() {
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
  load_institutions();
  load_regions();
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
  born_ = false;
  template_list_.push_back(this);
  parent_ = NULL;
  parentID_ = -1;
  MLOG(LEV_DEBUG3) << "Model ID=" << ID_ << ", ptr=" << this << " created.";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model::~Model() {
  MLOG(LEV_DEBUG3) << "Deleting model '" << name() << "' ID=" << ID_ << " {";
  
  // set died on date and record it in the table
  diedOn_ = TI->time();
  data a_don(diedOn_);
  entry don("LeaveDate",a_don);
  agent_table->updateRow( this->pkref(), don );
  
  // remove references to self
  removeFromList(this, template_list_);
  removeFromList(this, model_list_);

  if (parent_ != NULL) {
    parent_->removeChild(this);
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
void Model::setParent(Model* parent){ 
  // A model is "born" in the world it's parent is set
  this->setBornOn( TI->time() );

  // log who the parent is
  if (parent == this) {
    // root nodes are their own parent
    parent_ = NULL; // parent pointer set to NULL for memory management
    parentID_ = this->ID();
  }
  else{
    parent_ = parent;
    parentID_ = parent->ID();
  }

  // register the model with the simulation
  this->addToTable();

  // the model has been registered with the simulation, 
  // so it is no longer a template
  setIsTemplate(false);
  
  // if this node is not its own parent, add it to its parent's list of children
  if (parent_ != NULL){
    parent_->addChild(this);
  }

  CLOG(LEV_DEBUG2) << "Created Model: {";
  CLOG(LEV_DEBUG2) << str();
  CLOG(LEV_DEBUG2) << "}";
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Model::parent(){
  // if parent pointer is null, throw an error
  if (parent_ == NULL){
    string null_err = "You have tried to access the parent of " +	\
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
  removeChild(child);
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
const string Model::modelImpl() {
  return model_impl_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
vector<rsrc_ptr> Model::removeResource(Transaction order) {
  string msg = "The model " + name();
  msg += " doesn't support resource removal.";
  throw CycOverrideException(msg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::addResource(Transaction trans,
			std::vector<rsrc_ptr> manifest) {
  string err_msg = "The model " + name();
  err_msg += " doesn't support resource receiving.";
  throw CycOverrideException(err_msg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::define_table() {
  // declare the table columns
  column agent_id("ID","INTEGER");
  column agent_type("Type","VARCHAR(128)");
  column parent_id("ParentID","INTEGER");
  column bornOn("EnterDate","INTEGER");
  column diedOn("LeaveDate","INTEGER");
  // declare the table's primary key
  agent_table->setPrimaryKey(agent_id);
  // add columns to the table
  agent_table->addColumn(agent_id);
  agent_table->addColumn(agent_type);
  agent_table->addColumn(parent_id);
  agent_table->addColumn(bornOn);
  agent_table->addColumn(diedOn);
  // add foreign keys
  foreign_key_ref *fkref;
  foreign_key *fk;
  key myk, theirk;
  //    Agent table foreign keys
  theirk.push_back("ID");
  fkref = new foreign_key_ref("Agents",theirk);
  //      the parent id
  myk.push_back("ParentID");
  fk = new foreign_key(myk, (*fkref) );
  agent_table->addForeignKey( (*fk) ); // parentid references' agents' id
  // we've now defined the table
  agent_table->tableDefined();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::addToTable(){
  // if we haven't logged an agent yet, define the table
  if ( !agent_table->defined() )
    Model::define_table();

  // make a row
  // declare data
  data an_id( this->ID() ), a_type( this->modelImpl() ), 
    a_pid( this->parentID() ), a_bod( this->bornOn() );
  // declare entries
  entry id("ID",an_id), type("Type",a_type), pid("ParentID",a_pid), 
    bod("EnterDate",a_bod);
  // declare row
  row aRow;
  aRow.push_back(id), aRow.push_back(type), aRow.push_back(pid),
    aRow.push_back(bod);
  // add the row
  agent_table->addRow(aRow);
  // record this primary key
  pkref_.push_back(id);
}
