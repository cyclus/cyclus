#if !defined _TESTINST_H_
#define _TESTINST_H_

#include <sstream>

#include "InstModel.h"

/**
   This is the simplest possible Institution, for testing
 */
class TestInst: public InstModel {
 public:
  virtual ~TestInst() {};
  
  void receiveMessage(msg_ptr msg); 

  bool canBuild(Model* mdl);

  void build(Model* mdl, Model* parent) ;

  void copyFreshModel(Model* model) ;

 public:
  void wrapAddPrototype(Model* prototype);
};

#endif
