#if !defined _TESTINST_H_
#define _TESTINST_H_

#include "InstModel.h"

#include "CycException.h"

#include <sstream>

/**
   This is the simplest possible Institution, for testing
 */
class TestInst: public InstModel {
 public:
  virtual ~TestInst() {};
  
  void receiveMessage(msg_ptr msg) {
    msg->setDir(DOWN_MSG);
  }

  bool canBuild(Model* mdl) {return true;}

 public:
  void wrapAddPrototype(Model* prototype){addPrototype(prototype);}
};

#endif
