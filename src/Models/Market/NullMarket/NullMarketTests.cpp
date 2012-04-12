// NullMarketTests.cpp
#include <gtest/gtest.h>

#include "NullMarket.h"
#include "CycException.h"
#include "Message.h"
#include "MarketModelTests.h"
#include "GenericResource.h"
#include "ModelTests.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeNullMarket : public NullMarket {
  protected:
    msg_ptr msg_;
  public:
    FakeNullMarket() : NullMarket() {
      string kg = "kg";
      string qual = "qual";
      gen_rsrc_ptr res = gen_rsrc_ptr(new GenericResource(kg, qual, 1));
      msg_ = msg_ptr(new Message(this));
      msg_->setResource(res);
    }

    virtual ~FakeNullMarket() {
    }

    msg_ptr getMessage(){return msg_;}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* NullMarketModelConstructor(){
  return dynamic_cast<Model*>(new FakeNullMarket());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MarketModel* NullMarketConstructor(){
  return dynamic_cast<MarketModel*>(new FakeNullMarket());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class NullMarketTest : public ::testing::Test {
  protected:
    FakeNullMarket* src_market;
    FakeNullMarket* new_market; 

    virtual void SetUp(){
      src_market = new FakeNullMarket();
      new_market = new FakeNullMarket();
    };

    virtual void TearDown() {
      delete src_market;
    }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(NullMarketTest, InitialState) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(NullMarketTest, CopyMarket) {
  new_market->copy(src_market); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(NullMarketTest, CopyFreshModel) {
  new_market->copyFreshModel(dynamic_cast<Model*>(src_market)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<NullMarket*>(new_market)); // still a source market
  EXPECT_NO_THROW(dynamic_cast<FakeNullMarket*>(new_market)); // still a fake source market
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(NullMarketTest, Print) {
  EXPECT_NO_THROW(std::string s = src_market->str());
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(NullMarketTest, ReceiveMessage) {
  EXPECT_NO_THROW(src_market->receiveMessage(src_market->getMessage()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(NullMarket, MarketModelTests, Values(&NullMarketConstructor));
INSTANTIATE_TEST_CASE_P(NullMarket, ModelTests, Values(&NullMarketModelConstructor));

