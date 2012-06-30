// ConditioningFacilityTests.cpp
#include <gtest/gtest.h>

#include "ConditioningFacility.h"
#include "CycException.h"
#include "Message.h"
#include "FacilityModelTests.h"
#include "ModelTests.h"
#include "TestMarket.h"

#include <string>
#include <deque>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeConditioningFacility : public ConditioningFacility {
  public:
    FakeConditioningFacility() : ConditioningFacility() {
    }

    virtual ~FakeConditioningFacility() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* ConditioningFacilityModelConstructor(){
  return dynamic_cast<Model*>(new FakeConditioningFacility());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel* ConditioningFacilityConstructor(){
  return dynamic_cast<FacilityModel*>(new FakeConditioningFacility());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ConditioningFacilityTest : public ::testing::Test {
  protected:
    FakeConditioningFacility* cond_facility;
    FakeConditioningFacility* new_facility; 
    deque<string> commods;
    deque<MarketModel*> markets;

    virtual void SetUp(){
      cond_facility = new FakeConditioningFacility();
      cond_facility->setParent(new TestInst());
      new_facility = new FakeConditioningFacility();
      commods.push_back("I_stream");
      commods.push_back("CsSr_stream");
      commods.push_back("Tc_stream");
      commods.push_back("Fp_stream");
      for(int i=0;i<commods.size();i++){
        markets.push_back(new TestMarket(commods.at(i)));
      }
    };

    virtual void TearDown() {
      delete cond_facility;
      delete new_facility;
      for(int i=0;i<markets.size();i++){
        delete markets.at(i);
      }
    }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(ConditioningFacilityTest, InitialState) {
  // Test things about the initial state of the facility here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(ConditioningFacilityTest, CopyFreshModel) {
  new_facility->copyFreshModel(dynamic_cast<Model*>(cond_facility)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<ConditioningFacility*>(new_facility)); // still a cond facility
  EXPECT_NO_THROW(dynamic_cast<FakeConditioningFacility*>(new_facility)); // still a fake cond facility
  // Test that ConditioningFacility specific parameters are initialized in the deep copy method here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(ConditioningFacilityTest, Print) {
  EXPECT_NO_THROW(std::string s = cond_facility->str());
  // Test ConditioningFacility specific aspects of the print method here
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(ConditioningFacilityTest, ReceiveMessage) {
  msg_ptr msg;
  // Test ConditioningFacility specific behaviors of the receiveMessage function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(ConditioningFacilityTest, Tick) {
  int time = 1;
  EXPECT_NO_THROW(cond_facility->handleTick(time));
  // Test ConditioningFacility specific behaviors of the handleTick function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(ConditioningFacilityTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(cond_facility->handleTick(time));
  // Test ConditioningFacility specific behaviors of the handleTock function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(CondFac, FacilityModelTests, Values(&ConditioningFacilityConstructor));
INSTANTIATE_TEST_CASE_P(CondFac, ModelTests, Values(&ConditioningFacilityModelConstructor));

