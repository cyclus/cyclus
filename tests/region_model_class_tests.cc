// Instmodel_tests.h
#include <gtest/gtest.h>

#include "context.h"
#include "recorder.h"
#include "inst_model.h"
#include "region_model.h"
#include "mock_region.h"
#include "timer.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - Tests specific to the InstModel class itself- - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class DieInst : public cyclus::InstModel {
 public:
  DieInst(cyclus::Context* ctx)
      : cyclus::InstModel(ctx),
        cyclus::Model(ctx)  {
    tickCount_ = 0;
    tockCount_ = 0;
    tickDie_ = false;
    tockDie_ = false;
  };
  
  virtual ~DieInst() {};
  
  virtual cyclus::Model* Clone() {
    return new DieInst(context());
  }

  virtual void Tick(int time) {
    tickCount_++;
    totalTicks++;

    if (tickDie_) {
      context()->DelModel(this);
    }
  }

  virtual void Tock(int time) {
    tockCount_++;
    totalTocks++;
    
    if (tockDie_) {
      context()->DelModel(this);
    }
  }

  int tickCount_;
  int tockCount_;

  bool tickDie_;
  bool tockDie_;

  static int totalTicks;
  static int totalTocks;
};

int DieInst::totalTicks = 0;
int DieInst::totalTocks = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class RegionModelClassTests : public ::testing::Test {
  protected:

    DieInst* child1_;
    DieInst* child2_;
    DieInst* child3_;
    DieInst* child4_;
    DieInst* child5_;

    cyclus::TimeListener* reg_;
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

      reg_ = new MockRegion(ctx_);
      child1_->Deploy(reg_);
      child2_->Deploy(reg_);
      child3_->Deploy(reg_);
      child4_->Deploy(reg_);
      child5_->Deploy(reg_);
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(RegionModelClassTests, TickIter) {
  child2_->tickDie_ = true;

  ASSERT_NO_THROW(reg_->Tick(0));
  EXPECT_EQ(DieInst::totalTicks, 5);
  EXPECT_EQ(child1_->tickCount_, 1);
  EXPECT_EQ(child3_->tickCount_, 1);
  EXPECT_EQ(child4_->tickCount_, 1);
  EXPECT_EQ(child5_->tickCount_, 1);

  child1_->tickDie_ = true;
  child3_->tickDie_ = true;

  ASSERT_NO_THROW(reg_->Tick(0));
  EXPECT_EQ(DieInst::totalTicks, 9);
  EXPECT_EQ(child4_->tickCount_, 2);
  EXPECT_EQ(child5_->tickCount_, 2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(RegionModelClassTests, TockIter) {
  child2_->tockDie_ = true;

  ASSERT_NO_THROW(reg_->Tock(0));
  EXPECT_EQ(DieInst::totalTocks, 5);
  EXPECT_EQ(child1_->tockCount_, 1);
  EXPECT_EQ(child3_->tockCount_, 1);
  EXPECT_EQ(child4_->tockCount_, 1);
  EXPECT_EQ(child5_->tockCount_, 1);

  child1_->tockDie_ = true;
  child3_->tockDie_ = true;

  ASSERT_NO_THROW(reg_->Tock(0));
  EXPECT_EQ(DieInst::totalTocks, 9);
  EXPECT_EQ(child4_->tockCount_, 2);
  EXPECT_EQ(child5_->tockCount_, 2);
}

