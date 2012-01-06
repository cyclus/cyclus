// SinkFacilityTests.cpp
#include <gtest/gtest.h>

#include "SinkFacility.h"
#include "CycException.h"
#include "Message.h"
#include "Model.h"
#include "FacilityModelTests.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeSinkFacility : public SinkFacility {
  public:
    FakeSinkFacility() : SinkFacility() {
      in_commods_.push_back("in-commod");
      capacity_ = 2;
      inventory_size_ = 50;
      commod_price_ = 5000;

      inventory_ = deque<Material*>();
    }

    virtual ~FakeSinkFacility() {
    }

    double fakeCheckInventory() { return checkInventory(); }

    std::string getInCommod() {return in_commods_.front();}
    double getCapacity() {return capacity_;}
    double getInvSize() {return inventory_size_;}
    double getCommodPrice() {return commod_price_;}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel* SinkFacilityConstructor(){
  return dynamic_cast<FacilityModel*>(new FakeSinkFacility());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SinkFacilityTest : public ::testing::Test {
  protected:
    FakeSinkFacility* sink_facility;
    FakeSinkFacility* new_facility; 
    TestMarket* commod_market;

    virtual void SetUp(){
      sink_facility = new FakeSinkFacility();
      sink_facility->setParent(new TestInst());
      new_facility = new FakeSinkFacility();
      commod_market = new TestMarket(sink_facility->getInCommod());
    };

    virtual void TearDown() {
      delete sink_facility;
      delete new_facility;
    }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SinkFacilityTest, InitialState) {
  int time = 1;
  EXPECT_DOUBLE_EQ(0.0, sink_facility->fakeCheckInventory());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SinkFacilityTest, CopyFacility) {
  new_facility->copy(sink_facility); 
  EXPECT_DOUBLE_EQ(0.0, new_facility->fakeCheckInventory()); // fresh inventory
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SinkFacilityTest, CopyFreshModel) {
  new_facility->copyFreshModel(dynamic_cast<Model*>(sink_facility)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<SinkFacility*>(new_facility)); // still a source facility
  EXPECT_NO_THROW(dynamic_cast<FakeSinkFacility*>(new_facility)); // still a fake source facility
  EXPECT_DOUBLE_EQ(0.0, new_facility->fakeCheckInventory()); // fresh inventory
  EXPECT_EQ(sink_facility->getCapacity(), new_facility->getCapacity());
  EXPECT_EQ(sink_facility->getInvSize(), new_facility->getInvSize());
  EXPECT_EQ(sink_facility->getCommodPrice(), new_facility->getCommodPrice());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SinkFacilityTest, Print) {
  EXPECT_NO_THROW(sink_facility->print());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SinkFacilityTest, ReceiveMessage) {
  Message* msg;
  EXPECT_NO_THROW(sink_facility->receiveMessage(msg));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SinkFacilityTest, Tick) {
  int time = 1;
  EXPECT_DOUBLE_EQ(0.0, sink_facility->fakeCheckInventory());
  EXPECT_NO_THROW(sink_facility->handleTick(time));
  EXPECT_DOUBLE_EQ(0.0,sink_facility->fakeCheckInventory());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SinkFacilityTest, Tock) {
  int time = 1;
  EXPECT_DOUBLE_EQ(0.0,sink_facility->fakeCheckInventory());
  EXPECT_NO_THROW(sink_facility->handleTock(time));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(SinkFac, FacilityModelTests, Values(&SinkFacilityConstructor));


