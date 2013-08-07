// windows_helper_functions.h
// This is the dynamic loading implementation for WINDOWS machines
#ifndef WINDOWSHELPERFUNCTIONS_H
#define WINDOWSHELPERFUNCTIONS_H

#include <windows.h>

#include "suffix.h"
#include "error.h"

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::OpenLibrary() {
  module_library_ = LoadLibrary(abs_path_.c_str());

  if (!module_library_) {
    std::string err_msg = "Unable to load model shared object file: ";
    err_msg += model_name;
    err_msg += ". Error code is: ";
    err_msg += GetLastError();
    throw IOError(err_msg);
  }

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::SetConstructor() {
  constructor_ = (ModelCtor*)
                 GetProcAddress(module_library_, constructor_name_.c_str());

  if (!constructor_) {
    string err_msg = "Unable to load model constructor: ";
    err_msg += GetLastError();
    throw IOError(err_msg);
  }

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::SetDestructor() {
  destructor_ = (ModelDtor*)
                GetProcAddress(module_library_, destructor_name_.c_str());

  if (!destructor_) {
    string err_msg = "Unable to load model constructor: ";
    err_msg += GetLastError();
    throw IOError(err_msg);
  }

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::CloseLibrary() {
  if (module_library_) {
    FreeLibrary(module_library_);
  }
}
} // namespace cyclus

#endif
