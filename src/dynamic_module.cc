// dynamic_module.cc

#include "dynamic_module.h"

#include "boost/filesystem.hpp"

#include "context.h"
#include "env.h"
#include "model.h"
#include "suffix.h"

#include DYNAMICLOADLIB

namespace fs = boost::filesystem;

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const std::string DynamicModule::Suffix() {
  return SUFFIX;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DynamicModule::DynamicModule(std::string name)
    : module_name_(name),
      constructor_name_("Construct" + name),
      module_library_(0),
      constructor_(0) {
  std::string lib_name = "lib" + module_name_ + Suffix();
  fs::path p;
  if (!Env::FindModuleLib(lib_name, p)) {
    throw IOError("Could not find library: " + lib_name);
  }
  abs_path_ = p.string();

  OpenLibrary();
  SetConstructor();
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
  return abs_path_;
}
}  // namespace cyclus
