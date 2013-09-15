// Instmodel_tests.h
#include <gtest/gtest.h>

#include "event_manager.h"
#include "facility_model.h"
#include "inst_model.h"
#include "timer.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - Tests specific to the InstModel class itself- - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ConcreteInstModel : public cyclus::InstModel {
 public:
  ConcreteInstModel(cyclus::Context* ctx) : cyclus::InstModel(ctx) { };
  
  virtual ~ConcreteInstModel() {};

  virtual cyclus::Model* clone() {
    return new ConcreteInstModel(context());
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class DieModel : public cyclus::FacilityModel {
 public:
  DieModel(cyclus::Context* ctx) : FacilityModel(ctx) {
    tickCount_ = 0;
    tockCount_ = 0;
    tickDie_ = false;
    tockDie_ = false;
  };
  
  virtual ~DieModel() {};

  virtual void ReceiveMessage(cyclus::Message::Ptr msg) { };
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
    }
  }

  virtual cyclus::Model* clone() {
    return new DieModel(context());
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
    cyclus::EventManager em_;
    cyclus::Timer ti_;
    cyclus::Context* ctx_;

    virtual void SetUp() {
      ctx_ = new cyclus::Context(&ti_, &em_);

      child1_ = new DieModel(ctx_);
      child2_ = new DieModel(ctx_);
      child3_ = new DieModel(ctx_);
      child4_ = new DieModel(ctx_);
      child5_ = new DieModel(ctx_);

      inst_ = new ConcreteInstModel(ctx_);
      child1_->Deploy(inst_);
      child2_->Deploy(inst_);
      child3_->Deploy(inst_);
      child4_->Deploy(inst_);
      child5_->Deploy(inst_);
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

