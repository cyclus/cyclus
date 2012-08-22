// WinLoadConstr.h
// This is the dynamic loading implementation for Win32 machines
#include "suffix.h"

#include "CycException.h"
#include "Env.h"

#include <windows.h>
#include <iostream>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mdl_ctor* Model::loadConstructor(std::string model_type, std::string model_name) {
  mdl_ctor* new_model;

  string start_path = Env::getInstallPath() + "/lib";

  std::string construct_fname = std::string("construct") + model_name;
  //  std::string destruct_fname = std::string("destruct") + model_name;

  model_name = start_path + "/Models/" + model_type + "/" + model_name +
    "/" + model_name+SUFFIX;
  HMODULE model = LoadLibrary(model_name.c_str());
  if (!model) {
    string err_msg = "Unable to load model shared object file: ";
    err_msg += model_name;
    err_msg += ". Error code is: ";
    err_msg += GetLastError();
    throw CycIOException(err_msg);
  }
  
  new_model = (mdl_ctor*) GetProcAddress(model, construct_fname.c_str());
  if (!new_model) {
    string err_msg = "Unable to load model constructor: ";
    err_msg += GetLastError();
    throw CycIOException(err_msg);
  }
  
  return new_model;
}
