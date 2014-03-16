// unix_helper_functions.h
// This is the dynamic loading implementation for UNIX machines
#ifndef CYCLUS_SRC_UNIX_HELPER_FUNCTIONS_H_
#define CYCLUS_SRC_UNIX_HELPER_FUNCTIONS_H_

#include <dlfcn.h>

#include "error.h"
#include "suffix.h"

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::OpenLibrary() {
  module_library_ = dlopen(abs_path_.c_str(), RTLD_LAZY);

  if (!module_library_) {
    std::string err_msg = "Unable to load model shared object file: ";
    err_msg  += dlerror();
    throw IOError(err_msg);
  }

  dlerror();  // reset errors
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::SetConstructor() {
  constructor_ = (ModelCtor*)
                 dlsym(module_library_, constructor_name_.c_str());

  if (!constructor_) {
    std::string err_msg = "Unable to load module constructor: ";
    err_msg  += dlerror();
    throw IOError(err_msg);
  }

  dlerror();  // reset errors
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
    dlerror();  // reset errors
  }
}

}  // namespace cyclus

#endif  // CYCLUS_SRC_UNIX_HELPER_FUNCTIONS_H_
