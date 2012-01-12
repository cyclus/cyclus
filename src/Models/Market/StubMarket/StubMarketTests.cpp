// StubMarketTests.cpp
#include <gtest/gtest.h>

#include "StubMarket.h"
#include "CycException.h"
#include "Message.h"
#include "MarketModelTests.h"
#include "ModelTests.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeStubMarket : public StubMarket {
  public:
    FakeStubMarket() : StubMarket() {
    }

    virtual ~FakeStubMarket() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class StubMarketTest : public ::testing::Test {
  protected:
    FakeStubMarket* src_market;
    FakeStubMarket* new_market; 

    virtual void SetUp(){
      src_market = new FakeStubMarket();
      new_market = new FakeStubMarket();
    };

    virtual void TearDown() {
      delete src_market;
      delete new_market;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* StubMarketModelConstructor(){
  return dynamic_cast<Model*>(new FakeStubMarket());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MarketModel* StubMarketConstructor(){
  return dynamic_cast<MarketModel*>(new FakeStubMarket());
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubMarketTest, InitialState) {
  EXPECT_TRUE(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubMarketTest, CopyMarket) {
  new_market->copy(src_market); 
  EXPECT_TRUE(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubMarketTest, CopyFreshModel) {
  new_market->copyFreshModel(dynamic_cast<Model*>(src_market)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<StubMarket*>(new_market)); // still a source market
  EXPECT_NO_THROW(dynamic_cast<FakeStubMarket*>(new_market)); // still a fake source market
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubMarketTest, Print) {
  EXPECT_NO_THROW(src_market->print());
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubMarketTest, ReceiveMessage) {
  Message* msg;
  EXPECT_NO_THROW(src_market->receiveMessage(msg));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(StubMarket, MarketModelTests, Values(&StubMarketConstructor));
INSTANTIATE_TEST_CASE_P(StubMarket, ModelTests, Values(&StubMarketModelConstructor));

