// DynamicModule.cpp

#include "DynamicModule.h"

#include "Env.h"
#include "suffix.h"
#include "Model.h"
#include "boost/filesystem.hpp"

#include DYNAMICLOADLIB

using namespace std;
namespace fs = boost::filesystem;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const std::string DynamicModule::suffix() {
  return SUFFIX;
}

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
  string lib_name = "lib" + module_name_ + suffix();
  fs::path p;
  if (!Env::findModuleLib(lib_name,p)) {
    throw CycIOException("Could not find library: " + lib_name);
  }
  abs_path_ = p.string();
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
  if (abs_path_.length() == 0) {
    setPath();
  }
  return abs_path_;
}
