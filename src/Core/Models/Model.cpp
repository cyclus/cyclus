// Model.cpp
// Implements the Model Class

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#include "Model.h"

#include "Logger.h"
#include "suffix.h"
#include "CycException.h"
#include "Env.h"
#include "InputXML.h"
#include "Timer.h"
#include "Resource.h"
#include "Table.h"
#include "Prototype.h"

#include DYNAMICLOADLIB

using namespace std;

// Default starting ID for all Models is zero.
int Model::next_id_ = 0;
// Database table for agents
table_ptr Model::agent_table = new Table("Agents"); 
// Model containers
vector<Model*> Model::model_list_;

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
Model* Model::getEntityViaConstructor(std::string model_type,
                                      xmlNodePtr cur) {
  string model_impl = XMLinput->get_xpath_name(cur,"model/*");
  mdl_ctor* model_constructor = 
    loadConstructor(model_type,model_impl);
  return model_constructor();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::initializeSimulationEntity(std::string model_type, 
                                       xmlNodePtr cur) {
  Model* model = getEntityViaConstructor(model_type,cur);
  model->init(cur);
  model_list_.push_back(model);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::initializePrototype(std::string model_type, 
                                xmlNodePtr cur) {
  Model* model = getEntityViaConstructor(model_type,cur);
  model->init(cur);
  Prototype::registerPrototype(model->name(),
                               dynamic_cast<Prototype*>(model));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::loadGlobalElements() {
  load_converters();
  load_markets();
  load_facilities();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::loadEntities() {
  load_institutions();
  load_regions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_markets() {
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/*/market");
  
  for (int i=0;i<nodes->nodeNr;i++) {
    initializeSimulationEntity("Market",nodes->nodeTab[i]);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_converters() {
  try {
    xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/*/converter");
    
    for (int i=0;i<nodes->nodeNr;i++) {
      initializeSimulationEntity("Converter",nodes->nodeTab[i]);
    }
  } catch (CycNullXPathException) {} // no converters
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_facilities() {
  xmlNodeSetPtr nodes;
  try {
     nodes = XMLinput->get_xpath_elements("/*/facilitycatalog");
    
    for (int i=0;i<nodes->nodeNr;i++){
      load_facilitycatalog(XMLinput->get_xpath_content(nodes->nodeTab[i], 
                                                       "filename"),
                           XMLinput->get_xpath_content(nodes->nodeTab[i], "namespace"),
                           XMLinput->get_xpath_content(nodes->nodeTab[i], "format"));
    }
  } catch (CycNullXPathException) {}; // no converters
  
  nodes = XMLinput->get_xpath_elements("/*/facility");
  
  for (int i=0;i<nodes->nodeNr;i++) {
    initializePrototype("Facility",nodes->nodeTab[i]);
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
    initializeSimulationEntity("Region",nodes->nodeTab[i]);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_institutions() {

  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/simulation/region/institution");
  
  for (int i=0;i<nodes->nodeNr;i++) {
    initializeSimulationEntity("Inst",nodes->nodeTab[i]);   
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::init(xmlNodePtr cur) {
  name_ = XMLinput->getCurNS() + XMLinput->get_xpath_content(cur,"name");
  CLOG(LEV_DEBUG1) << "Model '" << name_ << "' just created.";
  model_impl_ = XMLinput->get_xpath_name(cur, "model/*");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model::Model() {
  children_ = vector<Model*>();
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
  data a_don(diedOn_);
  entry don("LeaveDate",a_don);
  agent_table->updateRow( this->pkref(), don );
  
  // remove references to self
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
void Model::removeFromList(Model* model, std::vector<Model*> &mlist) {
  vector<Model*>::iterator it = find(mlist.begin(),mlist.end(),model);
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
  // set model-specific members
  parentID_ = parent->ID();
  setParent(parent);
  parent->addChild(this);
  bornOn_ = TI->time();

  // add model to the database
  this->addToTable();

  // inform user
  CLOG(LEV_DEBUG2) << "Model Entered Simulation: {";
  CLOG(LEV_DEBUG2) << str();
  CLOG(LEV_DEBUG2) << "}";
}

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Model::printChildren() {
  stringstream ss("");
  ss << "Children of " << name() << ":" << endl;
  for (int i = 0; i < children_.size(); i++) {
    vector<string> print_outs = getTreePrintOuts(children_.at(i));
    for (int j = 0; j < print_outs.size(); j++) {
      ss << "\t" << print_outs.at(j);
    }
  }
  return ss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<std::string> Model::getTreePrintOuts(Model* m) {
  vector<string> ret;
  stringstream ss("");
  ss << m->name() << endl;
  ret.push_back(ss.str());
  for (int i = 0; i < m->nChildren(); i++) {
    vector<string> outs = getTreePrintOuts(m->children(i));
    for (int j = 0; j < outs.size(); j++) {
      ss.str("");
      ss << "\t" << outs.at(j) << endl;
      ret.push_back(ss.str());
    }
  }
  return ret;
}

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
  agent_table->addField("ID","INTEGER");
  agent_table->addField("AgentType","VARCHAR(128)"); // e.g. Inst, Facility
  agent_table->addField("ModelType","VARCHAR(128)"); // e.g. BuildInst
  agent_table->addField("Prototype","VARCHAR(128)"); // e.g. Areva, AP1000
  agent_table->addField("ParentID","INTEGER");
  agent_table->addField("EnterDate","INTEGER");
  agent_table->addField("LeaveDate","INTEGER");
  // declare the table's primary key
  agent_table->setPrimaryKey("ID");
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
  data an_id( this->ID() ), 
       an_agent_type( this->modelType() ), 
       a_model_type( this->modelImpl() ), 
       a_prototype( this->name() ), 
       a_pid( this->parentID() ), 
       a_bod( this->bornOn() );
  // declare entries
  entry id("ID",an_id), 
        agent_type("AgentType",an_agent_type), 
        model_type("ModelType",a_model_type), 
        prototype("Prototype",a_prototype), 
        pid("ParentID",a_pid), 
        bod("EnterDate",a_bod);
  // declare row
  row aRow;
  aRow.push_back(id), 
    aRow.push_back(agent_type), 
    aRow.push_back(model_type), 
    aRow.push_back(prototype), 
    aRow.push_back(pid),
    aRow.push_back(bod);
  // add the row
  agent_table->addRow(aRow);
  // record this primary key
  pkref_.push_back(id);
}
