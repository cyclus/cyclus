// Model.cpp
// Implements the Model Class

#include "suffix.h"
#include "Model.h"

#include "GenException.h"
#include "InputXML.h"
#include "Logician.h"

#include "RegionModel.h"

#include <dlfcn.h>


using namespace std;

map<string, mdl_ctor*> Model::create_map_;
map<string, mdl_dtor*> Model::destroy_map_;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mdl_ctor* Model::load(string model_type, string model_name) {
  mdl_ctor* new_model;

  model_name = "Models/" + model_type + "/lib" + model_name+SUFFIX;

  if (create_map_.find(model_name) == create_map_.end()) {
    void* model = dlopen(model_name.c_str(),RTLD_LAZY);
    if (!model) {
      throw GenException((string)"Unable to load model: " + dlerror() );
    }
    
    new_model = (mdl_ctor*) dlsym(model,"construct");
    if (!new_model) {
      throw GenException((string)"Unable to load model's create symbol: " + 
                         dlerror() );
    }

    mdl_dtor* del_model = (mdl_dtor*) dlsym(model,"destruct");
    if (!del_model) {
      throw GenException((string)"Unable to load model delete symbol: " + 
                         dlerror()  );
    }
  
    create_map_[model_name] = new_model;
    destroy_map_[model_name] = del_model;
  } else {
    new_model = create_map_[model_name];
  }

  return new_model;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Model::create(string model_type, xmlNodePtr cur) {
  string model_impl = XMLinput->get_xpath_name(cur, "model/*");

  mdl_ctor* model_creator = load(model_type, model_impl);

  Model* model = model_creator();

  model->init(cur);

  return model;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Model::create(Model* src) {
  mdl_ctor* model_creator = load(src->getModelType(),src->getModelImpl());
  
  Model* model = model_creator();
  
  model->copyFreshModel(src);

  return model;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void* Model::destroy(Model* model) {
  mdl_dtor* model_destructor = destroy_map_[model->getModelImpl()];

  model_destructor(model);

  return model;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string Model::generateHandle() {

  string toRet = model_impl_;

	char SNString[100];
	sprintf(SNString, "%d", ID_); 

	toRet.append(SNString);

  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::init(xmlNodePtr cur) {
  name_ = XMLinput->getCurNS() + XMLinput->get_xpath_content(cur,"name");
  model_impl_ = XMLinput->get_xpath_name(cur, "model/*");
  handle_ = this->generateHandle();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::copy(Model* src) {
  if (src->getModelType() != model_type_ && 
       src->getModelImpl() != model_impl_) {
    throw GenException("Cannot copy a model of type " 
        + src->getModelType() + "/" + src->getModelImpl()
        + " to an object of type "
        + model_type_ + "/" + model_impl_);
  }

  name_ = src->getName();
  model_impl_ = src->getModelImpl();
  handle_ = this->generateHandle();
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
    Model* thisFac = create("Facility",nodes->nodeTab[i]);
    LI->addModel(thisFac, FACILITY);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_facilitycatalog(string filename, string ns, string format){
  XMLinput->extendCurNS(ns);

  if ("xml" == format){
    XMLinput->load_facilitycatalog(filename);
  }
  else
    throw GenException(format + "is not a supported facilitycatalog format.");

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
void Model::print() { 
  cout << model_type_ << " " << name_ 
      << " (ID=" << ID_
      << ", implementation = " << model_impl_
      << "  handle = " << handle_
      << " ) " ;
};

