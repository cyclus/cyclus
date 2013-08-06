#if !defined _TESTINST_H_
#define _TESTINST_H_

#include "inst_model.h"

#include "error.h"

#include <sstream>

/**
   This is the simplest possible Institution, for testing
 */
class TestInst: public cyclus::InstModel {
 public:
  virtual ~TestInst() {};
  
  void ReceiveMessage(cyclus::msg_ptr msg) {
    msg->SetDir(cyclus::DOWN_MSG);
  }

  bool CanBuild(cyclus::Model* mdl) {return true;}

  void WrapAddAvailablePrototype(cyclus::Prototype* prototype) {
    AddAvailablePrototype(prototype);
  }
};

#endif
