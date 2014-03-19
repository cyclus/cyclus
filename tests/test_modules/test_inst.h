#ifndef CYCLUS_TESTS_TEST_MODULES_TEST_INST_H_
#define CYCLUS_TESTS_TEST_MODULES_TEST_INST_H_

#include "cyclus.h"

#include <sstream>

/**
   This is the simplest possible Institution, for testing
 */
class TestInst: public cyclus::Institution {
 public:
  TestInst(cyclus::Context* ctx) : cyclus::Institution(ctx) {};
  virtual ~TestInst() {};
  #pragma cyclus
};

#endif  // CYCLUS_TESTS_TEST_MODULES_TEST_INST_H_
