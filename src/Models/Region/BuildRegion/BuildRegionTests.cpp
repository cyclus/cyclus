// BuildRegionTests.cpp
#include <gtest/gtest.h>

#include "BuildRegion.h"

#include "TestInst.h"
#include "TestFacility.h"

#include "RegionModelTests.h"
#include "ModelTests.h"

#include <list>
#include <map>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeBuildRegion : public BuildRegion {
public:
  int nfacs, time_to_build;
  TestInst* inst;
  TestInst* inst2;
  TestFacility* fac;
  PrototypeDemand* test_order;

public:
  FakeBuildRegion() : BuildRegion() {}
  virtual ~FakeBuildRegion() {}
  
  void init() {
    // instantiate members
    inst = new TestInst();
    fac = new TestFacility();
    nfacs = 2;
    time_to_build = 1;

    // initialize members
    inst->setParent(this);
    fac->setParent(inst);
      
    // initalize orders
    PrototypeDemand d (fac,nfacs);
    test_order = new PrototypeDemand(d);
    PrototypeBuildOrder b (time_to_build,d);
    prototypeOrders_ = new PrototypeOrders();
    prototypeOrders_->push_back(b);

    // initialize builders
    builders_ = new map<Model*,list<Model*>*>();
    list<Model*>* a_list = new list<Model*>();
    a_list->push_back(inst);
    builders_->insert( pair<Model*,list<Model*>*>(fac,a_list) );
  }

  void initTwoInst() {
    init();
    inst2 = new TestInst();
    inst2->setParent(this);
    (*builders_)[fac]->push_back(inst2);
  }
  
  void wraphandlePrototypeOrder(PrototypeDemand order) {
    handlePrototypeOrder(order);
  }

  void wrapgetAvailableBuilders(Model* prototype, 
                                std::list<ModelIterator>& bidders) {
    getAvailableBuilders(prototype,bidders);
  }

  Model* wrapselectBuilder(Model* prototype, 
                           std::list<ModelIterator>& bidders) {
    return selectBuilder(prototype,bidders);
  }

  void wrapplaceOrder(Model* prototype, Model* builder) {
    placeOrder(prototype,builder);
  }

  Model* frontBuilder() { return (*builders_)[fac]->front(); }
  
  Model* backBuilder() { return (*builders_)[fac]->back(); }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class BuildRegionTest : public ::testing::Test {
protected:
  FakeBuildRegion* reg_;
  FakeBuildRegion* new_region_; 
  
  virtual void SetUp(){
    reg_ = new FakeBuildRegion();
    new_region_ = new FakeBuildRegion();
  };
  
  virtual void TearDown() {
    delete reg_;
    delete new_region_;
  }
  
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* BuildRegionModelConstructor(){
  return dynamic_cast<Model*>(new FakeBuildRegion());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RegionModel* BuildRegionConstructor(){
  return dynamic_cast<RegionModel*>(new FakeBuildRegion());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildRegionTest, CopyFreshModel) {
  // deep copy
  new_region_->copyFreshModel(dynamic_cast<Model*>(reg_)); 
  // still a build region
  EXPECT_NO_THROW(dynamic_cast<BuildRegion*>(new_region_));
  // still a fake build region
  EXPECT_NO_THROW(dynamic_cast<FakeBuildRegion*>(new_region_));  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildRegionTest, GetAvailableBuilders) {
  reg_->init();
  list<ModelIterator> bidders;
  EXPECT_NO_THROW(reg_->wrapgetAvailableBuilders(reg_->fac,bidders));
  EXPECT_EQ( (*bidders.front()), reg_->inst );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildRegionTest, SelectBuilder) {
  reg_->init();
  list<ModelIterator> bidders;
  EXPECT_NO_THROW(reg_->wrapgetAvailableBuilders(reg_->fac,bidders));
  Model* builder;
  EXPECT_NO_THROW(builder = 
                  reg_->wrapselectBuilder(reg_->fac,bidders));
  EXPECT_EQ( builder, reg_->inst );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildRegionTest, BuilderOrder) {
  reg_->initTwoInst();

  // expect order to be inst, inst2
  EXPECT_EQ( reg_->frontBuilder(), reg_->inst );
  EXPECT_EQ( reg_->backBuilder(), reg_->inst2 );

  // go through selection process
  list<ModelIterator> bidders;
  EXPECT_NO_THROW(reg_->wrapgetAvailableBuilders(reg_->fac,bidders));
  Model* builder;
  EXPECT_NO_THROW(builder = 
                  reg_->wrapselectBuilder(reg_->fac,bidders));
  
  // expect order to be inst2, inst
  EXPECT_EQ( reg_->frontBuilder(), reg_->inst2 );
  EXPECT_EQ( reg_->backBuilder(), reg_->inst );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildRegionTest, PlaceOrder) {
  reg_->init();
  EXPECT_NO_THROW(reg_->wrapplaceOrder(reg_->fac,reg_->inst));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildRegionTest, HandlePrototypeOrder) {
  reg_->init();
  EXPECT_NO_THROW(reg_->wraphandlePrototypeOrder( (*reg_->test_order) ));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildRegionTest, HandleTick) {
  reg_->init();
  int time = reg_->time_to_build;
  EXPECT_NO_THROW(reg_->handleTick(time));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATE_TEST_CASE_P(BuildRegion, RegionModelTests, 
                        Values(&BuildRegionConstructor));
INSTANTIATE_TEST_CASE_P(BuildRegion, ModelTests, 
                        Values(&BuildRegionModelConstructor));

