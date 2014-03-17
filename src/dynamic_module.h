// dynamic_module.h
#ifndef CYCLUS_SRC_DYNAMIC_MODULE_H_
#define CYCLUS_SRC_DYNAMIC_MODULE_H_

#include <string>
#include <map>

namespace cyclus {

class Agent;
class Context;

typedef Agent* AgentCtor(Context*);

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

  /// Returns a newly constructed model for the given module name.
  static Agent* Make(Context* ctx, std::string name);

  /// Closes all statically loaded dynamic modules. This should always be called
  /// before process termination.  This must be called AFTER all models have
  /// been destructed.
  static void CloseAll();

  /// @return the global library suffix
  static const std::string Suffix();

  /// @return the module name
  std::string name();

  /// If this path for this module has not been discovered yet, path searches
  /// for it.
  ///
  /// @exception IOError the library path was not found
  /// @return the filepath of the dynamic library.
  std::string path();

 private:
  /// Creates a new dynamically loadable module.
  /// @param name the name of the module
  DynamicModule(std::string name);

  /// construct an instance of this module
  /// @return a fresh instance
  Agent* ConstructInstance(Context* ctx);

  /// closes the loaded module dynamic lib
  void CloseLibrary();

  /// all dynamically loaded modules are
  /// added to this map when loaded.
  static std::map<std::string, DynamicModule*> modules_;

  /// the path to the library
  std::string abs_path_;

  /// the name of the module
  std::string module_name_;

  /// the name of all module constructor functions
  std::string constructor_name_;

  /// the library to open and close
  void* module_library_;

  /// a functor for the constructor
  AgentCtor* constructor_;

  /// uses dlopen to open the module shared lib
  void OpenLibrary();

  /// sets the constructor member
  void SetConstructor();
};
}  // namespace cyclus

#endif  // CYCLUS_SRC_DYNAMIC_MODULE_H_
