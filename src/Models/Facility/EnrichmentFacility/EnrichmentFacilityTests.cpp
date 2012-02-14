// EnrichmentFacilityTests.cpp
#include <gtest/gtest.h>

#include "EnrichmentFacility.h"
#include "CycException.h"
#include "Message.h"
#include "FacilityModelTests.h"
#include "ModelTests.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeEnrichmentFacility : public EnrichmentFacility {
  public:
    FakeEnrichmentFacility() : EnrichmentFacility() {
      in_commod_ = "in-commod";
      out_commod_= "out-commod";
      inventory_size_ = 10;
      capacity_ = 2;
      default_xw_ = 0.3;
      outstMF_ = 0;
      
      inventory_ = deque<mat_rsrc_ptr>();
      stocks_ = deque<mat_rsrc_ptr>();
      ordersWaiting_ = deque<msg_ptr>();
      ordersExecuting_ = ProcessLine();
    }

    virtual ~FakeEnrichmentFacility() {
    }

    virtual std::string getOutCommod(){return out_commod_;}
    virtual std::string getInCommod(){return in_commod_;}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* EnrichmentModelConstructor(){
  return dynamic_cast<Model*>(new FakeEnrichmentFacility());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel* EnrichmentFacilityConstructor(){
  return dynamic_cast<FacilityModel*>(new FakeEnrichmentFacility());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class EnrichmentFacilityTest : public ::testing::Test {

  public: 
  virtual void SetUp(){
    src_facility_ = new FakeEnrichmentFacility();
    test_inst_ = new TestInst();
    src_facility_->setParent(test_inst_);
    new_facility_ = new FakeEnrichmentFacility();
    in_market_ = new TestMarket(src_facility_->getInCommod());
    out_market_ = new TestMarket(src_facility_->getOutCommod());
  }

  virtual void TearDown() {
    delete src_facility_;
    delete new_facility_;
    delete test_inst_;
    delete out_market_;
    delete in_market_;
  }

  protected:
  FakeEnrichmentFacility* src_facility_;
  FakeEnrichmentFacility* new_facility_; 
  TestMarket* in_market_; 
  TestMarket* out_market_; 
  TestInst* test_inst_; 

};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(EnrichmentFacilityTest, InitialState) {
  // Test things about the initial state of the facility here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(EnrichmentFacilityTest, CopyFreshModel) {
  new_facility_->copyFreshModel(dynamic_cast<Model*>(src_facility_)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<EnrichmentFacility*>(new_facility_)); // still a enrichment facility
  EXPECT_NO_THROW(dynamic_cast<FakeEnrichmentFacility*>(new_facility_)); // still a fake enrichment facility
  // Test that EnrichmentFacility specific parameters are initialized in the deep copy method here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(EnrichmentFacilityTest, Print) {
  EXPECT_NO_THROW(src_facility_->print());
  // Test EnrichmentFacility specific aspects of the print method here
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(EnrichmentFacilityTest, ReceiveMessage) {
  msg_ptr msg;
  // Test EnrichmentFacility specific behaviors of the receiveMessage function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(EnrichmentFacilityTest, Tick) {
  int time = 1;
  EXPECT_NO_THROW(src_facility_->handleTick(time));
  // Test EnrichmentFacility specific behaviors of the handleTick function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(EnrichmentFacilityTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_facility_->handleTick(time));
  // Test EnrichmentFacility specific behaviors of the handleTock function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(EnrichmentFac, FacilityModelTests, Values(&EnrichmentFacilityConstructor));
INSTANTIATE_TEST_CASE_P(EnrichmentFac, ModelTests, Values(&EnrichmentModelConstructor));

