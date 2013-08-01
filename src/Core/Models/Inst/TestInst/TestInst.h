#if !defined _TESTINST_H_
#define _TESTINST_H_

#include "InstModel.h"

#include "error.h"

#include <sstream>

/**
   This is the simplest possible Institution, for testing
 */
class TestInst: public cyclus::InstModel {
 public:
  virtual ~TestInst() {};
  
  void receiveMessage(cyclus::msg_ptr msg) {
    msg->setDir(cyclus::DOWN_MSG);
  }

  bool canBuild(cyclus::Model* mdl) {return true;}

  void wrapAddAvailablePrototype(cyclus::Prototype* prototype) {
    addAvailablePrototype(prototype);
  }
};

#endif
