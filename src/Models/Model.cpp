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
	if (!model) {
	    string error_string = "Unable to load model: ";
	    error_string += dlerror();
	    throw GenException(error_string);
	}

	new_model = (mdl_ctor*) dlsym(model,"construct");
	if (!new_model) {
	    string error_string = "Unable to load model create symbol: ";
	    error_string += dlerror();
	    throw GenException(error_string);
	}
	mdl_dtor* del_model = (mdl_dtor*) dlsym(model,"destruct");
	if (!del_model) {
	    string error_string = "Unable to load model delete symbol: ";
	    error_string += dlerror();
	    throw GenException(error_string);
	}
	
	create_map[model_name] = new_model;
	destroy_map[model_name] = del_model;
    } else {
	new_model = create_map[model_name];
    }

    return new_model;
}


Model* Model::create(string model_type,xmlNodePtr cur)
{
    string name = XMLinput->get_xpath_content(cur, "name");
    string modelImpl = XMLinput->get_xpath_name(cur, "model/*");

    mdl_ctor* model_creator = load(model_type,modelImpl);

    Model* model = model_creator(cur);

    return model;
}

void* Model::destroy(Model* model)
{

    mdl_dtor* model_destructor = destroy_map[model->getModelName()];

    model_destructor(model);

    return model;

}

void Model::load_markets()
{

    xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/simulation/market");
    
    for (int i=0;i<nodes->nodeNr;i++)
	LI->addMarket(create("Market",nodes->nodeTab[i]));
}

void Model::load_facilities()
{

    xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/simulation/facility");
    
    for (int i=0;i<nodes->nodeNr;i++)
	LI->addFacility(create("Facility",nodes->nodeTab[i]));
}

void Model::load_regions()
{

    xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/simulation/region");
    
    for (int i=0;i<nodes->nodeNr;i++)
	LI->addRegion(create("Region",nodes->nodeTab[i]));
}

void Model::load_institutions()
{

    xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/simulation/region/institution");
    
    for (int i=0;i<nodes->nodeNr;i++)
	create("Inst",nodes->nodeTab[i]);
 
   
   
}
