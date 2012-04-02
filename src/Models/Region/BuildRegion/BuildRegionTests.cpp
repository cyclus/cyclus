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
  TestInst* inst;
  TestInst* inst2;
  TestFacility* fac;

  int nfacs, time_to_build_;
  PrototypeBuildOrder* test_order;
  PrototypeBuildOrder* test_order2;
  PrototypeOrders* orders_;

public:
  FakeBuildRegion() : BuildRegion() {}
  virtual ~FakeBuildRegion() {}
  
  void initSimple() {
    // instantiate members
    inst = new TestInst();
    inst->init();
    fac = new TestFacility();
    nfacs = 2;
    time_to_build_ = 1;

    // initialize members
    inst->setParent(this);
    inst->wrapAddPrototype(fac);
    fac->setParent(inst);
    builders_ = new map<Model*,list<Model*>*>();

    // initalize orders
    prototypeOrders_ = new PrototypeOrders();
    orders_ = new PrototypeOrders();
    PrototypeDemand d(fac,nfacs);
    test_order = new PrototypeBuildOrder(time_to_build_,d);
    orders_->push_back(test_order);
  }

  void init() {
    initSimple();
    wrapPopulateOrders(orders_);
    wrapPopulateBuilders();
  }

  void initTwoOrder() {
    initSimple();
    PrototypeDemand d(fac,nfacs);
    test_order2 = new PrototypeBuildOrder(time_to_build_-1,d);
    orders_->push_back(test_order2);
  }

  void initTwoInst() {
    init();
    inst2 = new TestInst();
    inst2->setParent(this);
    (*builders_)[fac]->push_back(inst2);
  }

  // Wrapping Methods for Protected Functions
  void wrapPopulateOrders(PrototypeOrders* orders) {
    populateOrders(orders);
  }

  void wrapSortOrders() { sortOrders(); }

  void wrapPopulateBuilders() { 
    populateBuilders(); 
  }
  
  void wrapHandlePrototypeOrder(PrototypeDemand order) {
    handlePrototypeOrder(order);
  }

  void wrapGetAvailableBuilders(Model* prototype, 
                                std::list<ModelIterator>& bidders) {
    getAvailableBuilders(prototype,bidders);
  }

  Model* wrapSelectBuilder(Model* prototype, 
                           std::list<ModelIterator>& bidders) {
    return selectBuilder(prototype,bidders);
  }

  void wrapPlaceOrder(Model* prototype, Model* builder) {
    placeOrder(prototype,builder);
  }

  // Wrapping Methods for Preotected Members
  Model* frontBuilder() { return (*builders_)[fac]->front(); }
  
  Model* backBuilder() { return (*builders_)[fac]->back(); }

  bool findFacInBuilders(Model* f) { 
    return builders_->find(f) != builders_->end(); 
  }

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
TEST_F(BuildRegionTest, PopulateOrders) {
  reg_->initSimple();
  EXPECT_NO_THROW( reg_->wrapPopulateOrders(reg_->orders_) );
  EXPECT_EQ( reg_->test_order, reg_->frontOrder() );
  EXPECT_EQ( reg_->test_order, reg_->backOrder() );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildRegionTest, SortOrders) {
  reg_->initTwoOrder();
  EXPECT_NO_THROW( reg_->wrapPopulateOrders(reg_->orders_) );
  EXPECT_EQ( reg_->test_order, reg_->frontOrder() );
  EXPECT_EQ( reg_->test_order2, reg_->backOrder() );
  EXPECT_NO_THROW( reg_->wrapSortOrders() );
  EXPECT_EQ( reg_->test_order2, reg_->frontOrder() );
  EXPECT_EQ( reg_->test_order, reg_->backOrder() );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildRegionTest, PopulateBuilders) {
  reg_->initSimple();
  EXPECT_NO_THROW( reg_->wrapPopulateBuilders() );
  EXPECT_EQ( reg_->findFacInBuilders(reg_->fac), true );
  EXPECT_EQ( reg_->inst, reg_->frontBuilder() );
  EXPECT_EQ( reg_->inst, reg_->backBuilder() );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildRegionTest, GetAvailableBuilders) {
  reg_->init();
  list<ModelIterator> bidders;
  EXPECT_NO_THROW(reg_->wrapGetAvailableBuilders(reg_->fac,bidders));
  EXPECT_EQ( (*bidders.front()), reg_->inst );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildRegionTest, SelectBuilder) {
  reg_->init();
  list<ModelIterator> bidders;
  EXPECT_NO_THROW(reg_->wrapGetAvailableBuilders(reg_->fac,bidders));
  Model* builder;
  EXPECT_NO_THROW(builder = 
                  reg_->wrapSelectBuilder(reg_->fac,bidders));
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
  EXPECT_NO_THROW(reg_->wrapGetAvailableBuilders(reg_->fac,bidders));
  Model* builder;
  EXPECT_NO_THROW(builder = 
                  reg_->wrapSelectBuilder(reg_->fac,bidders));
  
  // expect order to be inst2, inst
  EXPECT_EQ( reg_->frontBuilder(), reg_->inst2 );
  EXPECT_EQ( reg_->backBuilder(), reg_->inst );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildRegionTest, PlaceOrder) {
  reg_->init();
  EXPECT_NO_THROW(reg_->wrapPlaceOrder(reg_->fac,reg_->inst));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildRegionTest, HandlePrototypeOrder) {
  reg_->init();
  EXPECT_NO_THROW(reg_->
                  wrapHandlePrototypeOrder(reg_->frontOrder()->second));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildRegionTest, HandleTick) {
  reg_->init();
  int time = reg_->time_to_build_;
  EXPECT_NO_THROW(reg_->handleTick(time));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATE_TEST_CASE_P(BuildRegion, RegionModelTests, 
                        Values(&BuildRegionConstructor));
INSTANTIATE_TEST_CASE_P(BuildRegion, ModelTests, 
                        Values(&BuildRegionModelConstructor));

