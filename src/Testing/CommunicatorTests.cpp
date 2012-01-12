// StubCommModelTests.cpp 
#include <gtest/gtest.h>

#include "CommunicatorTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(CommunicatorTests, receiveMessage) {
  EXPECT_NO_THROW(comm_model_->receiveMessage(msg_));
}

