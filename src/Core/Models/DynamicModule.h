// DynamicModule.h
#ifndef DYNAMICMODULE_H
#define DYNAMICMODULE_H

#include <string>

class Model;

typedef Model* create_t();
typedef void destroy_t(Model*);

class DynamicModule {
 public:
  /**
     constructor
     sets:
     * module_name_
     * abs_path_
     * constructor_name_
     * destructor_name_
     @param type the module type
     @param name the name of the module
   */
  DynamicModule(std::string type, std::string name);
  
  /**
     destructor
     calls:
     * closeLibrary()
   */
  virtual ~DynamicModule();

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

  /**
     initializes the module by calling
     * setPath()
     * openLibrary()
     * setConstructor()
     * setDestructor()
   */
  void initialize();

  /**
     closes the library
   */
  void closeLibrary();
  
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
  void * module_library_;

  /// a functor for the constructor
  create_t * constructor_;
  
  /// a functor for the destructor
  destroy_t * destructor_;

  /// @return the path to the dynamic library
  std::string path();

  /**
     sets the absolute path to the dynamic library based on parameters set
     during construction
   */
  void setPath();

  /**
     opens the library
   */
  void openLibrary();

  /**
     sets the constructor member
   */
  void setConstructor();

  /**
     sets the destructor member
   */
  void setDestructor();
};

#endif
