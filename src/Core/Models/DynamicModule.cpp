// DynamicModule.cpp

#include "DynamicModule.h"

#include "Env.h"
#include "suffix.h"

DynamicModule::DynamicModule(std::string type, std::string name) {
  module_name_ = name;
  abs_path_ = Env::getInstallPath() + "/lib/Models/" + type + "/" +
    name + "/lib" + name + SUFFIX;
}

DynamicModule::~DynamicModule() {}

Model* DynamicModule::constructInstance() {
  return constructor_();
}

void DynamicModule::destructInstance(Model* model) {
  destructor_(model);
}

std::string DynamicModule::name() {
  return module_name_;
}

std::string DynamicModule::path() {
  return abs_path_;
}
