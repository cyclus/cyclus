#include <gtest/gtest.h>

#include "agent.h"

#include "../test_context.h"
#include "../test_agents/test_agent.h"

namespace cyclus {
namespace toolkit {

TEST(TimeSeriesTests, Power) {
  TestContext tc;
  Agent* a = new TestAgent(tc.get());
  RecordTimeSeries<POWER>(a, 42.0);
}

TEST(TimeSeriesTests, RawPower) {
  TestContext tc;
  Agent* a = new TestAgent(tc.get());
  RecordTimeSeries<double>("Power", a, 42.0);
}


}  // namespace toolkit
}  // namespace cyclus
