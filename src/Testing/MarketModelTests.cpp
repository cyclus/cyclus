// MarketModelTests.cpp 
#include <gtest/gtest.h>

#include "MarketModelTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(MarketModelTests, Print) {
  int time = 1;
  EXPECT_NO_THROW(market_model_->print());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(MarketModelTests, CopyMarket) {
  MarketModel* new_market = market_model_;
  EXPECT_NO_THROW(new_market->copy(market_model_)); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(MarketModelTests, CopyFreshMarket) {
  MarketModel* new_market = market_model_;
  EXPECT_NO_THROW(new_market->copyFreshModel(dynamic_cast<Model*>(market_model_))); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(MarketModelTests, ReceiveMessage) {
  EXPECT_NO_THROW(market_model_->resolve());
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(MarketModelTests, Resolve) {
  EXPECT_NO_THROW(market_model_->resolve());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(MarketModelTests, Commodity) {
  EXPECT_NO_THROW(market_model_->commodity());
}

