// Instmodel_tests.h
#include <gtest/gtest.h>

#include "context.h"
#include "event_manager.h"
#include "inst_model.h"
#include "region_model.h"
#include "timer.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - Tests specific to the InstModel class itself- - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ConcreteRegionModel : public cyclus::RegionModel {
 public:
  ConcreteRegionModel(cyclus::Context* ctx) : cyclus::RegionModel(ctx) { };
  
  virtual ~ConcreteRegionModel() { };
  
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class DieInst : public cyclus::InstModel {
 public:
  DieInst(cyclus::Context* ctx) : cyclus::InstModel(ctx) {
    tickCount_ = 0;
    tockCount_ = 0;
    tickDie_ = false;
    tockDie_ = false;
  };
  
  virtual ~DieInst() {};
  
  virtual void HandleTick(int time) {
    tickCount_++;
    totalTicks++;

    if (tickDie_) {
      delete this;
    }
  }

  virtual void HandleTock(int time) {
    tockCount_++;
    totalTocks++;
    
    if (tockDie_) {
      delete this;
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

    cyclus::TimeAgent* reg_;
    cyclus::EventManager em_;
    cyclus::Timer ti_;
    cyclus::Context* ctx_;

    virtual void SetUp() {
      ctx_ = new cyclus::Context(&ti_, &em_);

      child1_ = new DieInst(ctx_);
      child2_ = new DieInst(ctx_);
      child3_ = new DieInst(ctx_);
      child4_ = new DieInst(ctx_);
      child5_ = new DieInst(ctx_);

      reg_ = new ConcreteRegionModel(ctx_);
      child1_->EnterSimulation(reg_);
      child2_->EnterSimulation(reg_);
      child3_->EnterSimulation(reg_);
      child4_->EnterSimulation(reg_);
      child5_->EnterSimulation(reg_);
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(RegionModelClassTests, TickIter) {
  child2_->tickDie_ = true;

  ASSERT_NO_THROW(reg_->HandleTick(0));
  EXPECT_EQ(DieInst::totalTicks, 5);
  EXPECT_EQ(child1_->tickCount_, 1);
  EXPECT_EQ(child3_->tickCount_, 1);
  EXPECT_EQ(child4_->tickCount_, 1);
  EXPECT_EQ(child5_->tickCount_, 1);

  child1_->tickDie_ = true;
  child3_->tickDie_ = true;

  ASSERT_NO_THROW(reg_->HandleTick(0));
  EXPECT_EQ(DieInst::totalTicks, 9);
  EXPECT_EQ(child4_->tickCount_, 2);
  EXPECT_EQ(child5_->tickCount_, 2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(RegionModelClassTests, TockIter) {
  child2_->tockDie_ = true;

  ASSERT_NO_THROW(reg_->HandleTock(0));
  EXPECT_EQ(DieInst::totalTocks, 5);
  EXPECT_EQ(child1_->tockCount_, 1);
  EXPECT_EQ(child3_->tockCount_, 1);
  EXPECT_EQ(child4_->tockCount_, 1);
  EXPECT_EQ(child5_->tockCount_, 1);

  child1_->tockDie_ = true;
  child3_->tockDie_ = true;

  ASSERT_NO_THROW(reg_->HandleTock(0));
  EXPECT_EQ(DieInst::totalTocks, 9);
  EXPECT_EQ(child4_->tockCount_, 2);
  EXPECT_EQ(child5_->tockCount_, 2);
}

