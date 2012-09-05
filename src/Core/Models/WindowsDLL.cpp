// WindowsDLL.cpp

#include "WindowsDLL.h"

#include <windows.h>
#include "CycException.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
WindowsDLL::WindowsDLL(std::string type, std::string name) 
  : DynamicModule(type,name) {
  openLibrary();
  setConstructor();
  setDestructor();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
WindowsDLL::~WindowsDLL() {
  closeLibrary();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void WindowsDLL::openLibrary() {  
  module_library_ = LoadLibrary(abs_path_.c_str());

  if (!module_library_) {
    std::string err_msg = "Unable to load model shared object file: ";
    err_msg += model_name;
    err_msg += ". Error code is: ";
    err_msg += GetLastError();
    throw CycIOException(err_msg);
  }

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void WindowsDLL::setConstructor() {
  constructor_ = (create_t*) 
    GetProcAddress(module_library_,
                   DynamicModule::constructor_name.c_str());

  if (!constructor_) {
    string err_msg = "Unable to load model constructor: ";
    err_msg += GetLastError();
    throw CycIOException(err_msg);
  }

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void WindowsDLL::setDestructor() {
  destructor_ = (destroy_t*) 
    GetProcAddress(module_library_,
                   DynamicModule::destructor_name.c_str());

  if (!destructor_) {
    string err_msg = "Unable to load model constructor: ";
    err_msg += GetLastError();
    throw CycIOException(err_msg);
  }

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void WindowsDLL::closeLibrary() {
  if (module_library_)
    FreeLibrary(module_library_);
}
