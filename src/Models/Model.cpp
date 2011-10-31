// Model.cpp
// Implements the Model Class

#include "suffix.h"
#include "Model.h"

#include "CycException.h"
#include "Env.h"
#include "InputXML.h"
#include "Logician.h"

#include "RegionModel.h"

#include <dlfcn.h>
#include <iostream>


using namespace std;

// Default starting ID for all Models is zero.
int Model::next_id_ = 0;

map<string, mdl_ctor*> Model::create_map_;
map<string, mdl_dtor*> Model::destroy_map_;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Model::create(std::string model_type, xmlNodePtr cur) {
  string model_impl = XMLinput->get_xpath_name(cur, "model/*");

  // get instance
  mdl_ctor* model_constructor = loadConstructor(model_type, model_impl);

  Model* model = model_constructor();

  model->init(cur);

  return model;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Model::create(Model* model_orig) {
  mdl_ctor* model_constructor = loadConstructor(model_orig->getModelType(),model_orig->getModelImpl());
  
  Model* model_copy = model_constructor();
  
  model_copy->copyFreshModel(model_orig);

  return model_copy;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mdl_ctor* Model::loadConstructor(std::string model_type, std::string model_name) {
  mdl_ctor* new_model;

  string start_path = ENV->getCyclusPath();

  model_name = start_path + "/Models/" + model_type + "/lib" + 
               model_name+SUFFIX;

  if (create_map_.find(model_name) == create_map_.end()) {
    void* model = dlopen(model_name.c_str(),RTLD_LAZY);
    if (!model) {
      string err_msg = "Unable to load model shared object file: ";
      err_msg += dlerror();
      throw CycIOException(err_msg);
    }
    
    new_model = (mdl_ctor*) dlsym(model,"construct");
    if (!new_model) {
      string err_msg = "Unable to load model constructor: ";
      err_msg += dlerror();
      throw CycIOException(err_msg);
    }

    mdl_dtor* del_model = (mdl_dtor*) dlsym(model,"destruct");
    if (!del_model) {
      string err_msg = "Unable to load model destructor: ";
      err_msg += dlerror();
      throw CycIOException(err_msg);
    }
  
    create_map_[model_name] = new_model;
    destroy_map_[model_name] = del_model;
  } else {
    new_model = create_map_[model_name];
  }

  return new_model;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void* Model::destroy(Model* model) {
  mdl_dtor* model_destructor = destroy_map_[model->getModelImpl()];

  model_destructor(model);

  return model;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_markets() {
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/*/market");
  
  for (int i=0;i<nodes->nodeNr;i++) {
    LI->addModel(create("Market",nodes->nodeTab[i]), MARKET);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_converters() {

  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/*/converter");
  
  for (int i=0;i<nodes->nodeNr;i++) {
    LI->addModel(create("Converter",nodes->nodeTab[i]), CONVERTER);
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
    LI->addModel( create("Facility",nodes->nodeTab[i]), FACILITY );
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
    LI->addModel(create("Region",nodes->nodeTab[i]), REGION);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_institutions() {

  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/simulation/region/institution");
  
  for (int i=0;i<nodes->nodeNr;i++) {
    LI->addModel(create("Inst",nodes->nodeTab[i]), INST);   
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::init(xmlNodePtr cur) {
  name_ = XMLinput->getCurNS() + XMLinput->get_xpath_content(cur,"name");
  model_impl_ = XMLinput->get_xpath_name(cur, "model/*");
  handle_ = this->generateHandle();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::copy(Model* model_orig) {
  if (model_orig->getModelType() != model_type_ && 
       model_orig->getModelImpl() != model_impl_) {
    throw CycTypeException("Cannot copy a model of type " 
        + model_orig->getModelType() + "/" + model_orig->getModelImpl()
        + " to an object of type "
        + model_type_ + "/" + model_impl_);
  }

  name_ = model_orig->getName();
  model_impl_ = model_orig->getModelImpl();
  handle_ = this->generateHandle();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model::Model() {
  ID_ = ++next_id_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model::~Model() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::print() { 
  cout << model_type_ << " " << name_ 
      << " (ID=" << ID_
      << ", implementation = " << model_impl_
      << "  handle = " << handle_
      << " ) " ;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Model::parent(){
  if (parent_ == NULL){
    std::string null_err = "You have tried to access the parent of " +	\
      this->getName() + " but the parent point is NULL.";
    throw CycIndexException(null_err);
  }
  return parent_;
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::addChild(Model* child){ 
  child->setParent(this);
  children_.push_back(child); 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string Model::generateHandle() {

  string toRet = model_impl_;

	char SNString[100];
	sprintf(SNString, "%d", ID_); 

	toRet.append(SNString);

  return toRet;
}

