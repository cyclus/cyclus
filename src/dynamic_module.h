// dynamic_module.h
#ifndef CYCLUS_SRC_DYNAMIC_MODULE_H_
#define CYCLUS_SRC_DYNAMIC_MODULE_H_

#include <string>
#include <map>

#include "error.h"

namespace cyclus {

class Agent;
class Context;

typedef Agent* AgentCtor(Context*);

class InfileTree;

class AgentSpec {
 public:
  AgentSpec() {};
  AgentSpec(InfileTree* t);
  AgentSpec(std::string path, std::string lib, std::string agent, std::string alias);
  AgentSpec(std::string str_spec);

  std::string Sanitize();
  std::string LibPath();
  std::string str();

  std::string path() {return path_;};
  std::string lib() {return lib_;};
  std::string agent() {return agent_;};
  std::string alias() {return alias_;};

 private:
  std::string path_;
  std::string lib_;
  std::string agent_;
  std::string alias_;
};

std::string SanitizeSpec(std::string module_spec);

std::string ModuleConstructor(std::string module_spec);

class DynamicModule {
 public:
  /// Convenience class for using a stack variable to auto-destruct all loaded
  /// dynamic modules.
  class Closer {
   public:
    ~Closer() {
      CloseAll();
    };
  };

  /// Returns a newly constructed agent for the given module spec.
  static Agent* Make(Context* ctx, AgentSpec spec);

  /// Manually registers an agent to be used/cloned when Make is called.
  static void AddAgent(std::string spec, Agent* ref);

  /// Closes all statically loaded dynamic modules. This should always be called
  /// before process termination.  This must be called AFTER all agents have
  /// been destructed.
  static void CloseAll();

  /// The path to the module's shared object library.
  std::string path();

 private:
  /// Creates a new dynamically loadable module.
  /// @param name the name of the module
  DynamicModule(AgentSpec spec);

  /// construct an instance of this module
  /// @return a fresh instance
  Agent* ConstructInstance(Context* ctx);

  /// closes the loaded module dynamic lib
  void CloseLibrary();

  /// all dynamically loaded modules are
  /// added to this map when loaded.
  static std::map<std::string, DynamicModule*> modules_;

  static std::map<std::string, Agent*> man_agents_;

  /// the name of the module
  std::string path_;

  /// the name of the module
  std::string ctor_name_;

  /// the library to open and close
  void* module_library_;

  /// a functor for the constructor
  AgentCtor* ctor_;

  /// uses dlopen to open the module shared lib
  void OpenLibrary();

  /// sets the constructor member
  void SetConstructor();
};
}  // namespace cyclus

#endif  // CYCLUS_SRC_DYNAMIC_MODULE_H_
