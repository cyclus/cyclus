// StubCommModelTests.cpp 
#include <gtest/gtest.h>

#include "TimeAgentTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(TimeAgentTests, tick) {
  int time=1;
  EXPECT_NO_THROW(time_agent_->handleTick(time));
}

