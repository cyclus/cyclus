// dynamic_module.h
#ifndef DYNAMICMODULE_H
#define DYNAMICMODULE_H

#include <string>

namespace cyclus {

class Model;
class Context;

typedef Model* ModelCtor(Context*);

class DynamicModule {
 public:
  /// @return the global library suffix
  static const std::string Suffix();

  /// Creates a new dynamically loadable module.
  /// @param name the name of the module
  DynamicModule(std::string name);

  /// @return the module name
  std::string name();

  /// construct an instance of this module
  /// @return a fresh instance
  Model* ConstructInstance(Context* ctx);

  /// closes the loaded module dynamic lib
  void CloseLibrary();

  /// If this path for this module has not been discovered yet, path searches
  /// for it.
  ///
  /// @exception IOError the library path was not found
  /// @return the filepath of the dynamic library.
  std::string path();

 private:
  /// the path to the library
  std::string abs_path_;

  /// the name of the module
  std::string module_name_;

  /// the name of all module constructor functions
  std::string constructor_name_;

  /// the library to open and close
  void* module_library_;

  /// a functor for the constructor
  ModelCtor* constructor_;

  /// uses dlopen to open the module shared lib
  void OpenLibrary();

  /// sets the constructor member
  void SetConstructor();
};
} // namespace cyclus

#endif
