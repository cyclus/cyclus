// Instmodel_tests.h
#include <gtest/gtest.h>

#include "event_manager.h"
#include "facility_model.h"
#include "inst_model.h"
#include "mock_inst.h"
#include "timer.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - Tests specific to the InstModel class itself- - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class DieModel : public cyclus::FacilityModel {
 public:
  DieModel(cyclus::Context* ctx)
      : FacilityModel(ctx),
        cyclus::Model(ctx)  {
    tickCount_ = 0;
    tockCount_ = 0;
  };

  virtual ~DieModel() {};

  virtual void CloneModuleMembersFrom(FacilityModel* source) {};
  virtual void Decommission() {
    delete this;
  }

  virtual void HandleTick(int time) {
    tickCount_++;
    totalTicks++;
  }

  virtual void HandleTock(int time) {
    tockCount_++;
    totalTocks++;
  }

  virtual cyclus::Model* Clone() {
    return new DieModel(context());
  }

  int tickCount_;
  int tockCount_;

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

    inst_ = new MockInst(ctx_);
    child1_->Deploy(inst_);
    child2_->Deploy(inst_);
    child3_->Deploy(inst_);
    child4_->Deploy(inst_);
    child5_->Deploy(inst_);
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(InstModelClassTests, TockIter) {
  child2_->SetFacLifetime(0);

  EXPECT_EQ(inst_->children().size(), 5);
  ASSERT_NO_THROW(inst_->HandleTock(0));
  EXPECT_EQ(DieModel::totalTocks, 5);
  EXPECT_EQ(child1_->tockCount_, 1);
  EXPECT_EQ(child3_->tockCount_, 1);
  EXPECT_EQ(child4_->tockCount_, 1);
  EXPECT_EQ(child5_->tockCount_, 1);

  child1_->SetFacLifetime(0);
  child3_->SetFacLifetime(0);

  EXPECT_EQ(inst_->children().size(), 4);
  ASSERT_NO_THROW(inst_->HandleTock(1));
  EXPECT_EQ(DieModel::totalTocks, 9);
  EXPECT_EQ(child4_->tockCount_, 2);
  EXPECT_EQ(child5_->tockCount_, 2);
}

