// MarketModelTests.cpp 
#include <gtest/gtest.h>

#include "MarketModelTests.h"
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(MarketModelTests, Print) {
  int time = 1;
  EXPECT_NO_THROW(std::string s = market_model_->str());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(MarketModelTests, ReceiveMessage) {
  EXPECT_NO_THROW(market_model_->Resolve());
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(MarketModelTests, Resolve) {
  EXPECT_NO_THROW(market_model_->Resolve());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(MarketModelTests, Commodity) {
  EXPECT_NO_THROW(market_model_->commodity());
}

