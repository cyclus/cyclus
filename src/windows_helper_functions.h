// This is the dynamic loading implementation for WINDOWS machines
#ifndef CYCLUS_SRC_WINDOWS_HELPER_FUNCTIONS_H_
#define CYCLUS_SRC_WINDOWS_HELPER_FUNCTIONS_H_

#include <windows.h>

#include "error.h"
#include "suffix.h"

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::OpenLibrary() {
  module_library_ = LoadLibrary(path_.c_str());

  if (!module_library_) {
    std::string err_msg = "Unable to load agent shared object file: ";
    err_msg += ". Error code is: ";
    err_msg += GetLastError();
    throw IOError(err_msg);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::SetConstructor() {
  ctor_ = (AgentCtor*)GetProcAddress(module_library_, ctor_name_.c_str());

  if (!ctor_) {
    std::stringstream ss;
    std::string agent = ctor_name_;
    agent.erase(0, 9);  // len(Construct) == 9
    ss << "Could not find agent " << agent << " in module library "
       << path_.c_str() << " (" << GetLastError() << ").";
    throw IOError(ss.str());
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::CloseLibrary() {
  if (module_library_) {
    FreeLibrary(module_library_);
  }
}

}  // namespace cyclus

#endif  // CYCLUS_SRC_WINDOWS_HELPER_FUNCTIONS_H_
