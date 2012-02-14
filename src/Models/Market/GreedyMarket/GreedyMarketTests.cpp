// GreedyMarketTests.cpp
#include <gtest/gtest.h>

#include "GreedyMarket.h"
#include "CycException.h"
#include "Message.h"
#include "MarketModelTests.h"
#include "GenericResource.h"
#include "ModelTests.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeGreedyMarket : public GreedyMarket {
  protected:
    msg_ptr msg_;

  public:
    FakeGreedyMarket() : GreedyMarket() {
      string kg = "kg";
      string qual = "qual";
      gen_rsrc_ptr res = new GenericResource(kg, qual, 1);
      msg_ = msg_ptr(new Message(this));
      msg_->setResource(res);
    }

    virtual ~FakeGreedyMarket() {
    }

    msg_ptr getMessage(){return msg_;}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* GreedyMarketModelConstructor(){
  return dynamic_cast<Model*>(new FakeGreedyMarket());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MarketModel* GreedyMarketConstructor(){
  return dynamic_cast<MarketModel*>(new FakeGreedyMarket());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class GreedyMarketTest : public ::testing::Test {
  protected:
    FakeGreedyMarket* src_market;
    FakeGreedyMarket* new_market; 

    virtual void SetUp(){
      src_market = new FakeGreedyMarket();
      new_market = new FakeGreedyMarket();
    };

    virtual void TearDown() {
      delete src_market;
    }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(GreedyMarketTest, InitialState) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(GreedyMarketTest, CopyMarket) {
  new_market->copy(src_market); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(GreedyMarketTest, CopyFreshModel) {
  new_market->copyFreshModel(dynamic_cast<Model*>(src_market)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<GreedyMarket*>(new_market)); // still a source market
  EXPECT_NO_THROW(dynamic_cast<FakeGreedyMarket*>(new_market)); // still a fake source market
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(GreedyMarketTest, Print) {
  EXPECT_NO_THROW(src_market->print());
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(GreedyMarketTest, ReceiveMessage) {
  EXPECT_NO_THROW(src_market->receiveMessage(src_market->getMessage()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(GreedyMarket, MarketModelTests, Values(&GreedyMarketConstructor));
INSTANTIATE_TEST_CASE_P(GreedyMarket, ModelTests, Values(&GreedyMarketModelConstructor));

