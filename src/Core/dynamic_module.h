// dynamic_module.h
#ifndef DYNAMICMODULE_H
#define DYNAMICMODULE_H

#include <string>

namespace cyclus {

class Model;

typedef Model* ModelCtor();
typedef void ModelDtor(Model*);

class DynamicModule {
 public:
  /// @return the global library suffix
  static const std::string Suffix();

  /**
     Creates a new dynamically loadable module.
     @param type the module type
     @param name the name of the module
  */
  DynamicModule(std::string type, std::string name);

  /// @return the module name
  std::string name();

  /**
     construct an instance of this module
     @return a fresh instance
  */
  Model* ConstructInstance();

  /**
     call this destructor on an instance of a Model
     @param model the model to destruct
  */
  void DestructInstance(Model* model);

  /// Locates and initializes a dynamic module for use in the simulation
  void Initialize();

  /// closes the loaded module dynamic lib
  void CloseLibrary();

  /**
     If this path for this module has not been discovered yet, path searches for it.
     @exception IOError the library path was not found
     @return the filepath of the dynamic library.
  */
  std::string path();

 private:
  /// the path to the library
  std::string abs_path_;

  /// the name of the module
  std::string module_name_;

  /// the type of module
  std::string type_;

  /// the name of all module constructor functions
  std::string constructor_name_;

  /// the name of all module destructor functions
  std::string destructor_name_;

  /// the library to open and close
  void* module_library_;

  /// a functor for the constructor
  ModelCtor* constructor_;

  /// a functor for the destructor
  ModelDtor* destructor_;

  /// uses dlopen to open the module shared lib
  void OpenLibrary();

  /// sets the constructor member
  void SetConstructor();

  /// sets the destructor member
  void SetDestructor();

  /**
     sets the absolute path to the dynamic library based on parameters set
     during construction
  */
  void SetPath();

};
} // namespace cyclus

#endif
