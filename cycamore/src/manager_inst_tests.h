#ifndef CYCAMORE_SRC_MANAGER_INST_TESTS_H_
#define CYCAMORE_SRC_MANAGER_INST_TESTS_H_

#include <gtest/gtest.h>

#include "cyclus.h"
#include "timer.h"
#include "test_context.h"
#include "institution_tests.h"
#include "agent_tests.h"

#include "manager_inst.h"


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class TestProducer
    : public cyclus::Facility,
      public cyclus::toolkit::CommodityProducer {
 public:
  TestProducer(cyclus::Context* ctx);
  ~TestProducer();

  cyclus::Agent* Clone() {
    TestProducer* m = new TestProducer(context());
    m->InitFrom(this);
    return m;
  }

  void InitFrom(TestProducer* m) {
    cyclus::Facility::InitFrom(m);
  }

  void InitInv(cyclus::Inventories& inv) {}

  cyclus::Inventories SnapshotInv() { return cyclus::Inventories(); }

  void Tock() {}
  void Tick() {}
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ManagerInstTests : public ::testing::Test {
 public:
  virtual void SetUp();
  virtual void TearDown();

 protected:
  cycamore::ManagerInst* src_inst;
  TestProducer* producer;

  cyclus::toolkit::Commodity commodity;
  double capacity;
  cyclus::Context* ctx_;
  cyclus::Timer ti_;
  cyclus::Recorder rec_;
};

#endif  // CYCAMORE_SRC_MANAGER_INST_TESTS_H_
