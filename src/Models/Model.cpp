// Model.cpp
// Implements the Model Class

#include "Model.h"
#include "GenException.h"

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
