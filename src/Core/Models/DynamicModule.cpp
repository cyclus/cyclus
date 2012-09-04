// DynamicModule.cpp

#include "DynamicModule.h"

DynamicModule::DynamicModule(std::string name) : module_name_(name) {
  abs_path_ = getAbsPath();
  module_library_ = openLib();
  constructor_ = loadConstructor();
  destructor_ = loadDestructor();
}

DynamicModule::~DynamicModule() {
  closeLib(module_library_);
}

std::string DynamicModule::name() {
  return module_name_;
}
