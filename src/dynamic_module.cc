// dynamic_module.cc

#include "dynamic_module.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "context.h"
#include "env.h"
#include "agent.h"
#include "suffix.h"

#include DYNAMICLOADLIB

namespace fs = boost::filesystem;

namespace cyclus {

AgentSpec::AgentSpec(std::string path, std::string lib, std::string agent,
                     std::string alias)
  : path_(path), lib_(lib), agent_(agent), alias_(alias) {

  if (lib_ == "") {
    lib_ = agent_;
  }
  if (alias_ == "") {
    alias = agent_;
  }
}

AgentSpec::AgentSpec(InfileTree* t) {
  agent_ = t->GetString("name");
  lib_ = OptionalQuery<std::string>(t, "lib", agent_);
  path_ = OptionalQuery<std::string>(t, "path", "");
  alias_ = OptionalQuery<std::string>(t, "alias", agent_);
}

AgentSpec::AgentSpec(std::string str_spec) {
  std::vector<std::string> strs;
  boost::split(strs, str_spec, boost::is_any_of(":"));
  if (strs.size() != 3) {
    throw ValueError("invalid agent spec string '" + str_spec + "'");
  }
  path_ = strs[0];
  lib_ = strs[1];
  agent_ = strs[2];
  alias_ = agent_;
};

std::string AgentSpec::Sanitize() {
  std::string s = str();
  boost::replace_all(s, "/", "_");
  boost::replace_all(s, "-", "_");
  boost::replace_all(s, ":", "_");
  boost::replace_all(s, ".", "_");
  return s;
}

std::string AgentSpec::LibPath() {
  return (fs::path(path_) / fs::path("lib" + lib_ + SUFFIX)).string();
}

std::string AgentSpec::str() {
  return path_ + ":" + lib_ + ":" + agent_;
}

std::map<std::string, DynamicModule*> DynamicModule::modules_;
std::map<std::string, AgentCtor*> DynamicModule::man_ctors_;

Agent* DynamicModule::Make(Context* ctx, AgentSpec spec) {
  if (man_ctors_.count(spec.str()) > 0) { // for testing
    Agent* a = man_ctors_[spec.str()](ctx);
    a->spec(spec.str());
    return a;
  } else if (modules_.count(spec.str()) == 0) {
    DynamicModule* dyn = new DynamicModule(spec);
    modules_[spec.str()] = dyn;
  }

  DynamicModule* dyn = modules_[spec.str()];
  Agent* a = dyn->ConstructInstance(ctx);
  a->spec(spec.str());
  return a;
}

bool DynamicModule::Exists(AgentSpec spec) {
  bool rtn = true;
  try { 
    DynamicModule dyn(spec);
  } catch (cyclus::Error& e) {
    rtn = false;
  }
  return rtn;
}

void DynamicModule::CloseAll() {
  std::map<std::string, DynamicModule*>::iterator it;
  for (it = modules_.begin(); it != modules_.end(); it++) {
    it->second->CloseLibrary();
    delete it->second;
  }
  modules_.clear();
  man_ctors_.clear();
}

DynamicModule::DynamicModule(AgentSpec spec)
  : module_library_(0),
    ctor_(NULL) {
  path_ = Env::FindModule(spec.LibPath());
  ctor_name_ = "Construct" + spec.agent();
  OpenLibrary();
  SetConstructor();
}

Agent* DynamicModule::ConstructInstance(Context* ctx) {
  return ctor_(ctx);
}

std::string DynamicModule::path() {
  return path_;
}

}  // namespace cyclus

