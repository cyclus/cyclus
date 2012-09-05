// WindowsDLL.h
#ifndef WINDOWSDLL_H
#define WINDOWSDLL_H

#include <string>
#include "DynamicModule.h"

class WindowsDLL : public DynamicModule {
 public:
  /**
     constructor; sets module_name_
     NOTE: all derived constructors must:
     * set module_library_
     * call openLibrary()
     * set constructor_
     * set destructor_
   */
  WindowsDLL(std::string type, std::string name);
  
  /**
     destructor
     NOTE: all inherited destructors must:
     * call closeLibrary()
   */
  virtual ~WindowsDLL();

 protected:  
  /**
     all derived classes must define how to open the library and do
     so during their constructor
   */
  virtual void openLibrary();

  /**
     all derived classes must define how to set the constructor_ 
     member and must do so during their constructor
   */
  virtual void setConstructor();

  /**
     all derived classes must define how to set the destructor_ 
     member and must do so during their constructor
   */
  virtual void setDestructor();

  /**
     all derived classes must define how to close the library and
     do so during their destructor
   */
  virtual void closeLibrary();
};
#endif
