// Instmodel_tests.h
#include <gtest/gtest.h>

#include "context.h"
#include "recorder.h"
#include "inst_model.h"
#include "region_model.h"
#include "test_modules/test_region.h"
#include "timer.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - Tests specific to the InstModel class itself- - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class DieInst : public cyclus::InstModel {
 public:
  DieInst(cyclus::Context* ctx) : cyclus::InstModel(ctx) {
    tickDie_ = false;
    tockDie_ = false;
  };
  
  virtual ~DieInst() {};
  
  virtual cyclus::Model* Clone() {
    return new DieInst(context());
  }

  virtual void Tick(int time) {
    if (tickDie_) {
      context()->DelModel(this);
    }
  }

  virtual void Tock(int time) {
    if (tockDie_) {
      context()->DelModel(this);
    }
  }

  bool tickDie_;
  bool tockDie_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class RegionModelClassTests : public ::testing::Test {
  protected:

    DieInst* child1_;
    DieInst* child2_;
    DieInst* child3_;
    DieInst* child4_;
    DieInst* child5_;

    cyclus::RegionModel* reg_;
    cyclus::Recorder rec_;
    cyclus::Timer ti_;
    cyclus::Context* ctx_;

    virtual void SetUp() {
      ctx_ = new cyclus::Context(&ti_, &rec_);

      child1_ = new DieInst(ctx_);
      child2_ = new DieInst(ctx_);
      child3_ = new DieInst(ctx_);
      child4_ = new DieInst(ctx_);
      child5_ = new DieInst(ctx_);

      reg_ = new TestRegion(ctx_);
      child1_->Build(reg_);
      child2_->Build(reg_);
      child3_->Build(reg_);
      child4_->Build(reg_);
      child5_->Build(reg_);
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(RegionModelClassTests, TickIter) {
  ASSERT_EQ(5, reg_->children().size());

  child2_->tickDie_ = true;
  ASSERT_NO_THROW(child1_->Tick(0));
  ASSERT_NO_THROW(child2_->Tick(0));
  ASSERT_NO_THROW(child3_->Tick(0));
  ASSERT_NO_THROW(child4_->Tick(0));
  ASSERT_NO_THROW(child5_->Tick(0));
  EXPECT_EQ(4, reg_->children().size());

  child1_->tickDie_ = true;
  child3_->tickDie_ = true;
  ASSERT_NO_THROW(child1_->Tick(0));
  ASSERT_NO_THROW(child3_->Tick(0));
  ASSERT_NO_THROW(child4_->Tick(0));
  ASSERT_NO_THROW(child5_->Tick(0));
  EXPECT_EQ(2, reg_->children().size());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(RegionModelClassTests, TockIter) {
  ASSERT_EQ(5, reg_->children().size());

  child2_->tockDie_ = true;
  ASSERT_NO_THROW(child1_->Tock(0));
  ASSERT_NO_THROW(child2_->Tock(0));
  ASSERT_NO_THROW(child3_->Tock(0));
  ASSERT_NO_THROW(child4_->Tock(0));
  ASSERT_NO_THROW(child5_->Tock(0));
  EXPECT_EQ(4, reg_->children().size());

  child1_->tockDie_ = true;
  child3_->tockDie_ = true;
  ASSERT_NO_THROW(child1_->Tock(0));
  ASSERT_NO_THROW(child3_->Tock(0));
  ASSERT_NO_THROW(child4_->Tock(0));
  ASSERT_NO_THROW(child5_->Tock(0));
  EXPECT_EQ(2, reg_->children().size());
}

