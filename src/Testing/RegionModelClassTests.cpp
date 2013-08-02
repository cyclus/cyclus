// InstModelTests.h
#include <gtest/gtest.h>

#include "InstModel.h"
#include "RegionModel.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - Tests specific to the InstModel class itself- - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ConcreteRegionModel : public cyclus::RegionModel {
 public:
  ConcreteRegionModel() { };
  
  virtual ~ConcreteRegionModel() { };
  
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class DieInst : public cyclus::InstModel {
 public:
  DieInst() {
    tickCount_ = 0;
    tockCount_ = 0;
    tickDie_ = false;
    tockDie_ = false;
  };
  
  virtual ~DieInst() {};
  
  virtual void handleTick(int time) {
    tickCount_++;
    totalTicks++;

    if (tickDie_) {
      delete this;
    }
  }

  virtual void handleTock(int time) {
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

    virtual void SetUp() {
      child1_ = new DieInst();
      child2_ = new DieInst();
      child3_ = new DieInst();
      child4_ = new DieInst();
      child5_ = new DieInst();

      reg_ = new ConcreteRegionModel();
      child1_->enterSimulation(reg_);
      child2_->enterSimulation(reg_);
      child3_->enterSimulation(reg_);
      child4_->enterSimulation(reg_);
      child5_->enterSimulation(reg_);
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(RegionModelClassTests, TickIter) {
  child2_->tickDie_ = true;

  ASSERT_NO_THROW(reg_->handleTick(0));
  EXPECT_EQ(DieInst::totalTicks, 5);
  EXPECT_EQ(child1_->tickCount_, 1);
  EXPECT_EQ(child3_->tickCount_, 1);
  EXPECT_EQ(child4_->tickCount_, 1);
  EXPECT_EQ(child5_->tickCount_, 1);

  child1_->tickDie_ = true;
  child3_->tickDie_ = true;

  ASSERT_NO_THROW(reg_->handleTick(0));
  EXPECT_EQ(DieInst::totalTicks, 9);
  EXPECT_EQ(child4_->tickCount_, 2);
  EXPECT_EQ(child5_->tickCount_, 2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(RegionModelClassTests, TockIter) {
  child2_->tockDie_ = true;

  ASSERT_NO_THROW(reg_->handleTock(0));
  EXPECT_EQ(DieInst::totalTocks, 5);
  EXPECT_EQ(child1_->tockCount_, 1);
  EXPECT_EQ(child3_->tockCount_, 1);
  EXPECT_EQ(child4_->tockCount_, 1);
  EXPECT_EQ(child5_->tockCount_, 1);

  child1_->tockDie_ = true;
  child3_->tockDie_ = true;

  ASSERT_NO_THROW(reg_->handleTock(0));
  EXPECT_EQ(DieInst::totalTocks, 9);
  EXPECT_EQ(child4_->tockCount_, 2);
  EXPECT_EQ(child5_->tockCount_, 2);
}

