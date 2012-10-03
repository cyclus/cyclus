// InstModelTests.h
#include <gtest/gtest.h>

#include "InstModel.h"
#include "FacilityModel.h"
#include "Prototype.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - Tests specific to the InstModel class itself- - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ConcreteInstModel : public InstModel {
 public:
  ConcreteInstModel() { };
  
  virtual ~ConcreteInstModel() {};
  
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class DieModel : public FacilityModel {
 public:
  DieModel() {
    tickCount_ = 0;
    tockCount_ = 0;
    tickDie_ = false;
    tockDie_ = false;
  };
  
  virtual ~DieModel() {};
  
  // virtual Prototype* clone() {
  //   DieModel* clone = new DieModel();
  //   return dynamic_cast<Prototype*>(clone);
  // };

  virtual void receiveMessage(msg_ptr msg) { };

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

int DieModel::totalTicks = 0;
int DieModel::totalTocks = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class InstModelClassTests : public ::testing::Test {
  protected:

    DieModel* child1_;
    DieModel* child2_;
    DieModel* child3_;
    DieModel* child4_;
    DieModel* child5_;

    TimeAgent* inst_;

    virtual void SetUp() {
      child1_ = new DieModel();
      child2_ = new DieModel();
      child3_ = new DieModel();
      child4_ = new DieModel();
      child5_ = new DieModel();

      inst_ = new ConcreteInstModel();
      child1_->enterSimulation(inst_);
      child2_->enterSimulation(inst_);
      child3_->enterSimulation(inst_);
      child4_->enterSimulation(inst_);
      child5_->enterSimulation(inst_);
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(InstModelClassTests, TickIter) {
  child2_->tickDie_ = true;

  ASSERT_NO_THROW(inst_->handleTick(0));
  EXPECT_EQ(DieModel::totalTicks, 5);
  EXPECT_EQ(child1_->tickCount_, 1);
  EXPECT_EQ(child3_->tickCount_, 1);
  EXPECT_EQ(child4_->tickCount_, 1);
  EXPECT_EQ(child5_->tickCount_, 1);

  child1_->tickDie_ = true;
  child3_->tickDie_ = true;

  ASSERT_NO_THROW(inst_->handleTick(0));
  EXPECT_EQ(DieModel::totalTicks, 9);
  EXPECT_EQ(child4_->tickCount_, 2);
  EXPECT_EQ(child5_->tickCount_, 2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(InstModelClassTests, TockIter) {
  child2_->tockDie_ = true;

  ASSERT_NO_THROW(inst_->handleTock(0));
  EXPECT_EQ(DieModel::totalTocks, 5);
  EXPECT_EQ(child1_->tockCount_, 1);
  EXPECT_EQ(child3_->tockCount_, 1);
  EXPECT_EQ(child4_->tockCount_, 1);
  EXPECT_EQ(child5_->tockCount_, 1);

  child1_->tockDie_ = true;
  child3_->tockDie_ = true;

  ASSERT_NO_THROW(inst_->handleTock(0));
  EXPECT_EQ(DieModel::totalTocks, 9);
  EXPECT_EQ(child4_->tockCount_, 2);
  EXPECT_EQ(child5_->tockCount_, 2);
}

