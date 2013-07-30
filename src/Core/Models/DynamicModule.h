// DynamicModule.h
#ifndef DYNAMICMODULE_H
#define DYNAMICMODULE_H

#include <string>

class Model;

typedef Model* create_t();
typedef void destroy_t(Model*);

class DynamicModule {
 public:
  /// @return the global library suffix
  static const std::string suffix();
  
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
  Model* constructInstance();

  /**
     call this destructor on an instance of a Model
     @param model the model to destruct
  */
  void destructInstance(Model* model);

  /// Locates and initializes a dynamic module for use in the simulation
  void initialize();

  /// closes the loaded module dynamic lib
  void closeLibrary();

  /**
     If this path for this module has not been discovered yet, path searches for it.
     @exception CycIOException the library path was not found
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
  create_t* constructor_;

  /// a functor for the destructor
  destroy_t* destructor_;

  /// uses dlopen to open the module shared lib
  void openLibrary();

  /// sets the constructor member
  void setConstructor();

  /// sets the destructor member
  void setDestructor();

  /**
     sets the absolute path to the dynamic library based on parameters set
     during construction
  */
  void setPath();

};

#endif
