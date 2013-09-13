// dynamic_module.cc

#include "dynamic_module.h"

#include "context.h"
#include "env.h"
#include "suffix.h"
#include "model.h"
#include "boost/filesystem.hpp"

#include DYNAMICLOADLIB

namespace fs = boost::filesystem;

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const std::string DynamicModule::Suffix() {
  return SUFFIX;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DynamicModule::DynamicModule(std::string type, std::string name) :
  type_(type), module_name_(name),
  constructor_name_("Construct" + name), destructor_name_("Destruct" + name),
  abs_path_(""), module_library_(0), constructor_(0), destructor_(0) {

  SetPath();
  OpenLibrary();
  SetConstructor();
  SetDestructor();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DynamicModule::SetPath() {
  std::string lib_name = "lib" + module_name_ + Suffix();
  fs::path p;
  if (!Env::FindModuleLib(lib_name, p)) {
    throw IOError("Could not find library: " + lib_name);
  }
  abs_path_ = p.string();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* DynamicModule::ConstructInstance(Context* ctx) {
  return constructor_(ctx);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string DynamicModule::name() {
  return module_name_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string DynamicModule::path() {
  if (abs_path_.length() == 0) {
    SetPath();
  }
  return abs_path_;
}
} // namespace cyclus
