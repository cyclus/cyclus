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
  TestInst(cyclus::Context* ctx)
      : cyclus::InstModel(ctx),
        cyclus::Model(ctx) {};      

  virtual ~TestInst() {};

  virtual cyclus::Model* Clone() {return new TestInst(context());};

  void WrapAddAvailablePrototype(std::string proto_name) {
    AddAvailablePrototype(proto_name);
  }
};

#endif
