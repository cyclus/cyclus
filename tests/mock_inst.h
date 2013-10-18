#ifndef _CYCLUS_TESTS_MOCK_INST_H_
#define _CYCLUS_TESTS_MOCK_INST_H_

#include "context.h"
#include "inst_model.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MockInst : public cyclus::InstModel {
 public:
  MockInst(cyclus::Context* ctx)
      : cyclus::InstModel(ctx),
        cyclus::Model(ctx) { };
  
  virtual ~MockInst() {};

  virtual cyclus::Model* Clone() {
    MockInst* m = new MockInst(*this);
    m->InitFrom(this);
    return m;
  }
};

#endif // CYCLUS_TESTS_MOCK_INST_H_
