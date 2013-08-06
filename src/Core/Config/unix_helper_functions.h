// unix_helper_functions.h
// This is the dynamic loading implementation for UNIX machines
#ifndef UNIXHELPERFUNCTIONS_H
#define UNIXHELPERFUNCTIONS_H

#include <dlfcn.h>

#include "suffix.h"
#include "error.h"

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::OpenLibrary() {
  module_library_ = dlopen(abs_path_.c_str(),RTLD_LAZY);

  if (!module_library_) {
    std::string err_msg = "Unable to load model shared object file: ";
    err_msg  += dlerror();
    throw IOError(err_msg);
  }

  dlerror(); // reset errors
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::SetConstructor() {

  constructor_ = (create_t*) 
    dlsym(module_library_,constructor_name_.c_str());

  if (!constructor_) {
    std::string err_msg = "Unable to load module constructor: ";
    err_msg  += dlerror();
    throw IOError(err_msg);
  }

  dlerror(); // reset errors
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::SetDestructor() {
  destructor_ = (destroy_t*) 
    dlsym(module_library_,destructor_name_.c_str());

  if (!destructor_) {
    std::string err_msg = "Unable to load module destructor: ";
    err_msg  += dlerror();
    throw IOError(err_msg);
  }

  dlerror(); // reset errors
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::CloseLibrary() {
  if (module_library_) {
    int exit_code = dlclose(module_library_);
    if (exit_code != 0)  {
      std::string err_msg = "Error closing shared object file: ";
      err_msg  += dlerror();
      throw IOError(err_msg);
    }  
    dlerror(); // reset errors
  }
}
} // namespace cyclus

#endif
