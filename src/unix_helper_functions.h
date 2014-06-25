// This is the dynamic loading implementation for UNIX machines
#ifndef CYCLUS_SRC_UNIX_HELPER_FUNCTIONS_H_
#define CYCLUS_SRC_UNIX_HELPER_FUNCTIONS_H_

#include <dlfcn.h>

#include "error.h"
#include "suffix.h"

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::OpenLibrary() {
  module_library_ = dlopen(path_.c_str(), RTLD_LAZY);

  if (!module_library_) {
    std::string err_msg = "Unable to load agent shared object file: ";
    err_msg  += dlerror();
    throw IOError(err_msg);
  }

  dlerror();  // reset errors
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::SetConstructor() {
  ctor_ = (AgentCtor*)
                 dlsym(module_library_, ctor_name_.c_str());

  if (!ctor_) {
    std::stringstream ss;
    std::string agent = ctor_name_;
    agent.erase(0, 9);  // len(Construct) == 9
    ss << "Could not find agent " << agent << " in module library "
       << path_.c_str() << " (" << dlerror() << ").";
    throw IOError(ss.str());
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
