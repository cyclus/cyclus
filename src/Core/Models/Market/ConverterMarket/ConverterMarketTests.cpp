// ConverterMarketTests.cpp
#include <gtest/gtest.h>

#include "ConverterMarket.h"
#include "CycException.h"
#include "Message.h"
#include "MarketModelTests.h"
#include "GenericResource.h"
#include "ModelTests.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeConverterMarket : public ConverterMarket {
  protected:
    msg_ptr msg_;

  public:
    FakeConverterMarket() : ConverterMarket() {
      string kg = "kg";
      string qual = "qual";
      gen_rsrc_ptr res = gen_rsrc_ptr(new GenericResource(kg, qual, 1));
      Transaction trans(this, OFFER);
      msg_ = msg_ptr(new Message(this, this, trans));
      msg_->trans().setResource(res);
    }

    virtual ~FakeConverterMarket() {
    }

    msg_ptr getMessage() {return msg_;}

};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ConverterMarketTest : public ::testing::Test {
  protected:
    FakeConverterMarket* src_market;
    FakeConverterMarket* new_market; 

    virtual void SetUp(){
      src_market = new FakeConverterMarket();
      new_market = new FakeConverterMarket();
    };

    virtual void TearDown() {
      delete src_market;
      delete new_market;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* ConverterMarketModelConstructor(){
  return dynamic_cast<Model*>(new FakeConverterMarket());
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MarketModel* ConverterMarketConstructor(){
  return dynamic_cast<MarketModel*>(new FakeConverterMarket());
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(ConverterMarketTest, InitialState) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(ConverterMarketTest, CopyMarket) {
  new_market->copy(src_market); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(ConverterMarketTest, CopyFreshModel) {
  new_market->copyFreshModel(dynamic_cast<Model*>(src_market)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<ConverterMarket*>(new_market)); // still a source market
  EXPECT_NO_THROW(dynamic_cast<FakeConverterMarket*>(new_market)); // still a fake source market
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(ConverterMarketTest, Print) {
  EXPECT_NO_THROW(std::string s = src_market->str());
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(ConverterMarketTest, ReceiveMessage) {
  EXPECT_NO_THROW(src_market->receiveMessage(src_market->getMessage()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(ConverterMarket, MarketModelTests, Values(&ConverterMarketConstructor));
INSTANTIATE_TEST_CASE_P(ConverterMarket, ModelTests, Values(&ConverterMarketModelConstructor));

