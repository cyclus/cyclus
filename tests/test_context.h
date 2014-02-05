// test_context.h
#ifndef CYCLUS_TESTS_TEST_CONTEXT_H_
#define CYCLUS_TESTS_TEST_CONTEXT_H_

#include "context.h"
#include "recorder.h"
#include "timer.h"

namespace cyclus {
class TestContext {
 public:
  TestContext() {
    ctx_ = new Context(&ti_, &rec_);
  };

  ~TestContext() {
    delete ctx_;
  };
  
  Context* get() {return ctx_;}
  Timer* timer() {return &ti_;}
  Recorder* recorder() {return &rec_;}

 private:
  Timer ti_;
  Recorder rec_;
  Context* ctx_;
};
} // namespace cyclus

#endif  // CYCLUS_TESTS_TEST_CONTEXT_H_
