#ifndef CYCLUS_TESTS_TEST_MODULES_TEST_INST_H_
#define CYCLUS_TESTS_TEST_MODULES_TEST_INST_H_

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

#endif  // CYCLUS_TESTS_TEST_MODULES_TEST_INST_H_
