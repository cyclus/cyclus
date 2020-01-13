#ifndef CYCAMORE_SRC_SINK_TESTS_H_
#define CYCAMORE_SRC_SINK_TESTS_H_

#include <gtest/gtest.h>

#include "sink.h"

#include "test_context.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SinkTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc_;
  TestFacility* trader;
  cycamore::Sink* src_facility;
  std::string commod1_, commod2_, commod3_;
  double capacity_, inv_, qty_;
  int ncommods_;

  virtual void SetUp();
  virtual void TearDown();
  void InitParameters();
  void SetUpSink();
};

#endif  // CYCAMORE_SRC_SINK_TESTS_H_
