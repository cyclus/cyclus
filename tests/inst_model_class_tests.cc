// Instmodel_tests.h
#include <gtest/gtest.h>

#include "recorder.h"
#include "mock_facility.h"
#include "inst_model.h"
#include "mock_inst.h"
#include "timer.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - Tests specific to the InstModel class itself- - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class InstModelClassTests : public ::testing::Test {
 protected:

   cyclus::FacilityModel* child1_;
   cyclus::FacilityModel* child2_;
   cyclus::FacilityModel* child3_;
   cyclus::FacilityModel* child4_;
   cyclus::FacilityModel* child5_;

  cyclus::InstModel* inst_;
  cyclus::Recorder rec_;
  cyclus::Timer ti_;
  cyclus::Context* ctx_;

  virtual void SetUp() {
    ctx_ = new cyclus::Context(&ti_, &rec_);

    child1_ = new MockFacility(ctx_);
    child2_ = new MockFacility(ctx_);
    child3_ = new MockFacility(ctx_);
    child4_ = new MockFacility(ctx_);
    child5_ = new MockFacility(ctx_);

    inst_ = new MockInst(ctx_);
    child1_->Build(inst_);
    child2_->Build(inst_);
    child3_->Build(inst_);
    child4_->Build(inst_);
    child5_->Build(inst_);
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(InstModelClassTests, TockIter) {
  ASSERT_EQ(5, inst_->children().size());

  child2_->SetFacLifetime(0);
  ASSERT_NO_THROW(inst_->Tock(0));
  EXPECT_EQ(4, inst_->children().size());

  child1_->SetFacLifetime(0);
  child3_->SetFacLifetime(0);
  ASSERT_NO_THROW(inst_->Tock(1));
  EXPECT_EQ(2, inst_->children().size());
}

