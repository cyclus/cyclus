// Instmodel_tests.h
#include <gtest/gtest.h>

#include "inst_model.h"
#include "facility_model.h"
#include "prototype.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - Tests specific to the InstModel class itself- - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ConcreteInstModel : public cyclus::InstModel {
 public:
  ConcreteInstModel() { };
  
  virtual ~ConcreteInstModel() {};
  
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class DieModel : public cyclus::FacilityModel {
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

  virtual void ReceiveMessage(cyclus::msg_ptr msg) { };
  virtual void CloneModuleMembersFrom(FacilityModel* source){};
  virtual void Decommission() { delete this; }

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
      SetFacLifetime(1);
      SetBuildDate(time);
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

    cyclus::TimeAgent* inst_;

    virtual void SetUp() {
      child1_ = new DieModel();
      child2_ = new DieModel();
      child3_ = new DieModel();
      child4_ = new DieModel();
      child5_ = new DieModel();

      inst_ = new ConcreteInstModel();
      child1_->EnterSimulation(inst_);
      child2_->EnterSimulation(inst_);
      child3_->EnterSimulation(inst_);
      child4_->EnterSimulation(inst_);
      child5_->EnterSimulation(inst_);
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(InstModelClassTests, TickIter) {
  child2_->tickDie_ = true;

  ASSERT_NO_THROW(inst_->HandleTick(0));
  EXPECT_EQ(DieModel::totalTicks, 5);
  EXPECT_EQ(child1_->tickCount_, 1);
  EXPECT_EQ(child3_->tickCount_, 1);
  EXPECT_EQ(child4_->tickCount_, 1);
  EXPECT_EQ(child5_->tickCount_, 1);

  child1_->tickDie_ = true;
  child3_->tickDie_ = true;

  ASSERT_NO_THROW(inst_->HandleTick(0));
  EXPECT_EQ(DieModel::totalTicks, 9);
  EXPECT_EQ(child4_->tickCount_, 2);
  EXPECT_EQ(child5_->tickCount_, 2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(InstModelClassTests, TockIter) {
  child2_->tockDie_ = true;

  EXPECT_EQ(inst_->NChildren(),5);
  ASSERT_NO_THROW(inst_->HandleTock(0));
  EXPECT_EQ(DieModel::totalTocks, 5);
  EXPECT_EQ(child1_->tockCount_, 1);
  EXPECT_EQ(child3_->tockCount_, 1);
  EXPECT_EQ(child4_->tockCount_, 1);
  EXPECT_EQ(child5_->tockCount_, 1);

  child1_->tockDie_ = true;
  child3_->tockDie_ = true;

  EXPECT_EQ(inst_->NChildren(),4);
  ASSERT_NO_THROW(inst_->HandleTock(1));
  EXPECT_EQ(DieModel::totalTocks, 9);
  EXPECT_EQ(child4_->tockCount_, 2);
  EXPECT_EQ(child5_->tockCount_, 2);
}

