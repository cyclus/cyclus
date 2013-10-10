// test_context.h
#ifndef TEST_CONTEXT_H_
#define TEST_CONTEXT_H_

#include "context.h"
#include "event_manager.h"
#include "timer.h"

namespace cyclus {
class TestContext {
 public:
  TestContext() {
    ctx_ = new Context(&ti_, &em_);
  };

  ~TestContext() {
    delete ctx_;
  };
  
  Context* get() {return ctx_;};

 private:
  Timer ti_;
  EventManager em_;
  Context* ctx_;
};
} // namespace cyclus
#endif
