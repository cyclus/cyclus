// DynamicModule.h
#ifndef DYNAMICMODULE_H
#define DYNAMICMODULE_H

#include <string>

class Model;

typedef Model* create_t();
typedef void destroy_t(Model*);

class DynamicModule {
 public:
  
  DynamicModule(std::string name);
  
  virtual ~DynamicModule();

  std::string name();
  
 protected:

  virtual void * openLib() = 0;

  virtual std::string getAbsPath() = 0;

  virtual create_t * loadConstructor() = 0;

  virtual destroy_t * loadDestructor() = 0;

  virtual void closeLib(void * lib) = 0;

 private:
  
  void * module_library_;

  create_t * constructor_;
  
  destroy_t * destructor_;

  std::string abs_path_, module_name_;
};

#endif
