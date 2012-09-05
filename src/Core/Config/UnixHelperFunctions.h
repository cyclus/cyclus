// UnixHelperFunctions.h
// This is the dynamic loading implementation for UNIX machines
#ifndef UNIXHELPERFUNCTIONS_H
#define UNIXHELPERFUNCTIONS_H

#include <dlfcn.h>

#include "suffix.h"
#include "CycException.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::openLibrary() {
  module_library_ = dlopen(path().c_str(),RTLD_LAZY);

  if (!module_library_) {
    std::string err_msg = "Unable to load model shared object file: ";
    err_msg  += dlerror();
    throw CycIOException(err_msg);
  }

  dlerror(); // reset errors
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::setConstructor() {

  constructor_ = (create_t*) 
    dlsym(module_library_,DynamicModule::constructor_name.c_str());

  if (!constructor_) {
    std::string err_msg = "Unable to load module constructor: ";
    err_msg  += dlerror();
    throw CycIOException(err_msg);
  }

  dlerror(); // reset errors
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::setDestructor() {
  destructor_ = (destroy_t*) 
    dlsym(module_library_,DynamicModule::destructor_name.c_str());

  if (!destructor_) {
    std::string err_msg = "Unable to load module destructor: ";
    err_msg  += dlerror();
    throw CycIOException(err_msg);
  }

  dlerror(); // reset errors
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::closeLibrary() {
  if (module_library_)
    dlclose(module_library_);
}

#endif
