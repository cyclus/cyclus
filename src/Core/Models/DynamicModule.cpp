// DynamicModule.cpp

#include "DynamicModule.h"

#include "Env.h"
#include "suffix.h"
#include "Model.h"

#include DYNAMICLOADLIB

using namespace std;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DynamicModule::DynamicModule(std::string type,std::string name) :
  type_(type), module_name_(name), 
  constructor_name_("construct" + name), destructor_name_("destruct" + name),
  abs_path_(""), module_library_(0), constructor_(0), destructor_(0) 
{}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::initialize() {
  setPath();
  openLibrary();
  setConstructor();
  setDestructor();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DynamicModule::~DynamicModule() {
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::setPath() {
  abs_path_ = Env::getInstallPath() + "/lib/Models/" + type_ + "/" +
    module_name_ + "/lib" + module_name_ + SUFFIX;
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
