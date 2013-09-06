#if !defined _TESTINST_H_
#define _TESTINST_H_

#include "inst_model.h"

#include "error.h"
#include "context.h"

#include <sstream>

/**
   This is the simplest possible Institution, for testing
 */
class TestInst: public cyclus::InstModel {
 public:
  TestInst(cyclus::Context* ctx) : cyclus::InstModel(ctx) { };

  virtual ~TestInst() {};

  void ReceiveMessage(cyclus::Message::Ptr msg) {
    msg->SetDir(cyclus::DOWN_MSG);
  }

  void WrapAddAvailablePrototype(std::string proto_name) {
    AddAvailablePrototype(proto_name);
  }
};

#endif
