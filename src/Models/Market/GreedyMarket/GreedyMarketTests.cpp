// GreedyMarketTests.cpp
#include <gtest/gtest.h>

#include "GreedyMarket.h"
#include "CycException.h"
#include "Message.h"
#include "MarketModelTests.h"
#include "GenericResource.h"
#include "ModelTests.h"
#include "IsoVector.h"
#include "Material.h"
#include "FacilityModel.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeGreedyMarket : public GreedyMarket {
  protected:
    msg_ptr msg_;

  public:
    FakeGreedyMarket() {
      string kg = "kg";
      string qual = "qual";
      gen_rsrc_ptr res = gen_rsrc_ptr(new GenericResource(kg, qual, 1));
      Transaction trans(this, OFFER);
      msg_ = msg_ptr(new Message(this, this, trans));
      msg_->trans().setResource(res);
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
class FakeFacility: public FacilityModel {

public:

  std::vector<rsrc_ptr> received;
  std::vector<rsrc_ptr> sent;

  void copyFreshModel(Model* model) { };
  
  void receiveMessage(msg_ptr msg) {
    msg->approveTransfer();
  }

  void addResource(msg_ptr msg, vector<rsrc_ptr> manifest) {
    for (int i = 0; i < manifest.size(); i++) {
      received.push_back(manifest.at(i));
    }
  }

  std::vector<rsrc_ptr> removeResource(msg_ptr msg) {
    sent.push_back(msg->trans().resource());
    std::vector<rsrc_ptr> manifest;
    manifest.push_back(msg->trans().resource());
    return manifest;
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class GreedyMarketTest : public ::testing::Test {
  protected:
    FakeGreedyMarket* src_market;
    FakeGreedyMarket* new_market; 

    IsoVector recipe;
    msg_ptr offer, request;
    FakeFacility* supplier;
    FakeFacility* requester;

    virtual void SetUp(){
      recipe.setMass(92235, 10);
      recipe.setMass(92238, 90);

      src_market = new FakeGreedyMarket();
      new_market = new FakeGreedyMarket();

      Communicator* recipient = dynamic_cast<Communicator*>(src_market);

      supplier = new FakeFacility();
      requester = new FakeFacility();

      Transaction req(requester, REQUEST);
      req.setCommod("none");
      req.minfrac = 0.1;
      req.setPrice(3);

      Transaction off(supplier, OFFER);
      off.setCommod("none");
      off.minfrac = 0.1;
      off.setPrice(3);

      request = msg_ptr(new Message(requester, recipient, req)); 
      request->setNextDest(recipient);

      offer = msg_ptr(new Message(supplier, recipient, off)); 
      offer->setNextDest(recipient);

    };

    virtual void TearDown() {
      delete src_market;
      delete new_market;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(GreedyMarketTest, RequestIsLarger) {
  recipe.setMass(20);
  rsrc_ptr resource(new Material(recipe));
  request->trans().setResource(resource);

  recipe.setMass(15);
  resource = rsrc_ptr(new Material(recipe));
  offer->trans().setResource(resource);

  request->sendOn();
  offer->sendOn();
  src_market->resolve();

  EXPECT_EQ(supplier->received.size(), 0);
  EXPECT_EQ(supplier->sent.size(), 0);
  EXPECT_EQ(requester->received.size(), 0);
  EXPECT_EQ(requester->sent.size(), 0);

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(GreedyMarketTest, OfferIsLarger) {
  recipe.setMass(15);
  rsrc_ptr resource(new Material(recipe));
  request->trans().setResource(resource);

  recipe.setMass(20);
  resource = rsrc_ptr(new Material(recipe));
  offer->trans().setResource(resource);

  request->sendOn();
  offer->sendOn();
  src_market->resolve();

  EXPECT_EQ(supplier->received.size(), 0);
  EXPECT_EQ(supplier->sent.size(), 1);
  EXPECT_EQ(requester->received.size(), 1);
  EXPECT_EQ(requester->sent.size(), 0);

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(GreedyMarketTest, RequestEqOffer) {
  rsrc_ptr resource(new Material(recipe));
  request->trans().setResource(resource);
  offer->trans().setResource(resource);

  request->sendOn();
  offer->sendOn();
  src_market->resolve();

  EXPECT_EQ(supplier->received.size(), 0);
  EXPECT_EQ(supplier->sent.size(), 1);
  EXPECT_EQ(requester->received.size(), 1);
  EXPECT_EQ(requester->sent.size(), 0);

}

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
  EXPECT_NO_THROW(std::string s = src_market->str());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(GreedyMarketTest, ReceiveMessage) {
  EXPECT_NO_THROW(src_market->receiveMessage(src_market->getMessage()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(GreedyMarket, MarketModelTests, Values(&GreedyMarketConstructor));
INSTANTIATE_TEST_CASE_P(GreedyMarket, ModelTests, Values(&GreedyMarketModelConstructor));

