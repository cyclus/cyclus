#ifndef CYCAMORE_SRC_GROWTH_REGION_TESTS_H_
#define CYCAMORE_SRC_GROWTH_REGION_TESTS_H_
#include "platform.h"
#if CYCLUS_HAS_COIN

#include <gtest/gtest.h>

#include "context.h"
#include "growth_region.h"
#include "recorder.h"
#include "timer.h"

#include "test_context.h"
#include "region_tests.h"
#include "agent_tests.h"


namespace cycamore {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class GrowthRegionTests : public ::testing::Test {
 protected:
  cyclus::Context* ctx;
  cyclus::Timer ti;
  cyclus::Recorder rec;
  cycamore::GrowthRegion* region;
  std::string commodity_name, demand_type, demand_params, demand_start;

  virtual void SetUp();
  virtual void TearDown();
  bool ManagesCommodity(cyclus::toolkit::Commodity& commodity);
};

}  // namespace cycamore
#endif  // CYCLUS_HAS_COIN
#endif  // CYCAMORE_SRC_GROWTH_REGION_TESTS_H_
