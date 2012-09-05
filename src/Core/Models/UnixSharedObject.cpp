// UnixSharedObject.cpp

#include "UnixSharedObject.h"

#include <dlfcn.h>
#include "CycException.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
UnixSharedObject::UnixSharedObject(std::string type, std::string name) 
  : DynamicModule(type,name) {
  openLibrary();
  setConstructor();
  setDestructor();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
UnixSharedObject::~UnixSharedObject() {
  closeLibrary();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void UnixSharedObject::openLibrary() {
  module_library_ = dlopen(path().c_str(),RTLD_LAZY);

  if (!module_library_) {
    std::string err_msg = "Unable to load model shared object file: ";
    err_msg  += dlerror();
    throw CycIOException(err_msg);
  }

  dlerror(); // reset errors
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void UnixSharedObject::setConstructor() {

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
void UnixSharedObject::setDestructor() {
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
void UnixSharedObject::closeLibrary() {
  if (module_library_)
    dlclose(module_library_);
}
