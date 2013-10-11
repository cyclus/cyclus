#include <gtest/gtest.h>

#include "stub_market.h"

#include "market_model_tests.h"
#include "model_tests.h"

using stubs::StubMarket;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class StubMarketTest : public ::testing::Test {
  protected:
    cyclus::TestContext tc_;
    StubMarket* src_market_;

    virtual void SetUp(){
      src_market_ = new StubMarket(tc_.get());
    };

    virtual void TearDown() {
      delete src_market_;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubMarketTest, clone) {
  StubMarket* cloned_fac =
      dynamic_cast<StubMarket*> (src_market_->Clone());
  delete cloned_fac;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubMarketTest, InitialState) {
  // Test things about the initial state of the market here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubMarketTest, Print) {
  EXPECT_NO_THROW(std::string s = src_market_->str());
  // Test StubMarket specific aspects of the print method here
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubMarketTest, ReceiveMessage) {
  // Test StubMarket specific behaviors of the receiveMessage function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* StubMarketModelConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::Model*>(new StubMarket(ctx));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::MarketModel* StubMarketConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::MarketModel*>(new StubMarket(ctx));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(StubMarket, MarketModelTests,
                        ::testing::Values(&StubMarketConstructor));
INSTANTIATE_TEST_CASE_P(StubMarket, ModelTests,
                        ::testing::Values(&StubMarketModelConstructor));

