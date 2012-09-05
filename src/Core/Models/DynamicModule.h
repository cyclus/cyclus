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
     constructor; sets module_name_
     NOTE: all derived constructors must:
     * set module_library_
     * call openLibrary()
     * set constructor_
     * set destructor_
   */
  DynamicModule(std::string type, std::string name);
  
  /**
     destructor
     NOTE: all inherited destructors must:
     * call closeLibrary()
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
  
 protected:
  /// the library to open and close
  void * module_library_;

  /// a functor for the constructor
  create_t * constructor_;
  
  /// a functor for the destructor
  destroy_t * destructor_;

  /// @return the path to the dynamic library
  std::string path();

  /**
     all derived classes must define how to open the library and do
     so during their constructor
   */
  virtual void * openLibrary() = 0;

  /**
     all derived classes must define how to close the library and
     do so during their destructor
   */
  virtual void closeLibrary() = 0;

 private:
  /// the path to the library
  std::string abs_path_;
  
  /// the name of the module
  std::string module_name_;
};

#endif
