// Model.cpp
// Implements the Model Class

#include "Model.h"

#include "GenException.h"
#include "InputXML.h"
#include "Logician.h"

#include "RegionModel.h"

#include <dlfcn.h>


using namespace std;

map<string, mdl_ctor*> Model::create_map;
map<string, mdl_dtor*> Model::destroy_map;


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mdl_ctor* Model::load(string model_type,string model_name)
{
  mdl_ctor* new_model;

  model_name = "Models/" + model_type + "/lib" + model_name + ".so";

  if (create_map.find(model_name) == create_map.end()) {
    void* model = dlopen(model_name.c_str(),RTLD_LAZY);
    if (!model) throw GenException((string)"Unable to load model: " + dlerror() );
    
    new_model = (mdl_ctor*) dlsym(model,"construct");
    if (!new_model) throw GenException((string)"Unable to load model's create symbol: " + dlerror() );

    mdl_dtor* del_model = (mdl_dtor*) dlsym(model,"destruct");
    if (!del_model) throw GenException((string)"Unable to load model delete symbol: " + dlerror()  );
  
    create_map[model_name] = new_model;
    destroy_map[model_name] = del_model;
  } 
  else {
    new_model = create_map[model_name];
  }

  return new_model;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Model::create(string model_type,xmlNodePtr cur)
{
  string modelImpl = XMLinput->get_xpath_name(cur, "model/*");

  mdl_ctor* model_creator = load(model_type,modelImpl);

  Model* model = model_creator();

  model->init(cur);

  return model;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Model::create(Model* src)
{
  mdl_ctor* model_creator = load(src->model_type,src->modelImpl);
  
  Model* model = model_creator();
  
  model->copy(src);

  return model;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void* Model::destroy(Model* model)
{

  mdl_dtor* model_destructor = destroy_map[model->getModelName()];

  model_destructor(model);

  return model;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::init(xmlNodePtr cur)
{
  name = XMLinput->getCurNS() + XMLinput->get_xpath_content(cur,"name");
  modelImpl = XMLinput->get_xpath_name(cur, "model/*");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::copy(Model* src)
{
  if (src->model_type != model_type && src->modelImpl != modelImpl)
    throw GenException("Cannot copy a model of type " 
        + src->model_type + "/" + src->modelImpl
        + " to an object of type "
        + model_type + "/" + modelImpl);

  name = src->name;
  modelImpl = src->modelImpl;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_markets()
{

  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/*/market");
  
  for (int i=0;i<nodes->nodeNr;i++)
    LI->addMarket(create("Market",nodes->nodeTab[i]));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_converters()
{

  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/*/converter");
  
  for (int i=0;i<nodes->nodeNr;i++)
    LI->addConverter(create("Converter",nodes->nodeTab[i]));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_facilities()
{
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/*/facilitycatalog");
  
  for (int i=0;i<nodes->nodeNr;i++){
    load_facilitycatalog(XMLinput->get_xpath_content(nodes->nodeTab[i], "filename"),
        XMLinput->get_xpath_content(nodes->nodeTab[i], "namespace"),
        XMLinput->get_xpath_content(nodes->nodeTab[i], "format"));
  }

  nodes = XMLinput->get_xpath_elements("/*/facility");
  
  for (int i=0;i<nodes->nodeNr;i++){
    Model* thisFac = create("Facility",nodes->nodeTab[i]);
    LI->addFacility(thisFac);
  }
  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_facilitycatalog(string filename, string ns, string format)
{
  XMLinput->extendCurNS(ns);

  if ("xml" == format){
    XMLinput->load_facilitycatalog(filename);
  }
  else
    throw GenException(format + "is not a supported facilitycatalog format.");

  XMLinput->stripCurNS();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_regions()
{

  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/simulation/region");
  
  for (int i=0;i<nodes->nodeNr;i++)
    LI->addRegion(create("Region",nodes->nodeTab[i]));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::load_institutions()
{

  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/simulation/region/institution");
  
  for (int i=0;i<nodes->nodeNr;i++)
    LI->addInst(create("Inst",nodes->nodeTab[i]));   
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Model::print() 
{ 
  cout << model_type << " " << name 
      << " (ID=" << ID
      << ", implementation = " << modelImpl 
      << ") " ;
};

