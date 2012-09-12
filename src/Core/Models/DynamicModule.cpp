// DynamicModule.cpp

#include "DynamicModule.h"

#include "Env.h"
#include "suffix.h"
#include "Model.h"

#include DYNAMICLOADLIB

using namespace std;

// static variables

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DynamicModule::DynamicModule(std::string type, std::string name) :
  abs_path_(""), module_name_(""), 
  constructor_name_(""), destructor_name_(""),
  module_library_(0), constructor_(0), destructor_(0) {
  
  module_name_ = name;
  abs_path_ = Env::getInstallPath() + "/lib/Models/" + type + "/" +
    name + "/lib" + name + SUFFIX;
  constructor_name_="constructor";
  destructor_name_="destructor";

  openLibrary();
  setConstructor();
  setDestructor();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DynamicModule::~DynamicModule() {
  closeLibrary();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* DynamicModule::constructInstance() {
  return constructor_();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::destructInstance(Model* model) {
  destructor_(model);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string DynamicModule::name() {
  return module_name_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string DynamicModule::path() {
  return abs_path_;
}
