// SourceFacilityTests.cpp
#include <gtest/gtest.h>

#include "SourceFacility.h"
#include "CycException.h"
#include "Message.h"
#include "FacilityModelTests.h"
#include "ModelTests.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeSourceFacility : public SourceFacility {
  public:
    FakeSourceFacility() : SourceFacility() {
      out_commod_ = "out-commod";

      int u235 = 92235;
      double one = 1.0;
      string test_mat_unit = "test_mat_unit";
      string test_rec_name = "test_rec_name";
      double test_size = 10.0;
      bool test_template = true;
      IsoVector test_comp;
      test_comp.setMass(u235, one);

      recipe_ = IsoVector(test_comp);

      capacity_ = 2;
      inventory_size_ = 50;
      commod_price_ = 5000;

      inventory_ = deque<Material*>();
      ordersWaiting_ = deque<msg_ptr>();
    }

    virtual ~FakeSourceFacility() {
    }

    double fakeCheckInventory() { return checkInventory(); }

    std::string getOutCommod() {return out_commod_;}
    double getCapacity() {return capacity_;}
    double getInvSize() {return inventory_size_;}
    double getCommodPrice() {return commod_price_;}
    IsoVector getRecipe() {return recipe_;}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* SourceFacilityModelConstructor(){
  return dynamic_cast<Model*>(new FakeSourceFacility());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel* SourceFacilityConstructor(){
  return dynamic_cast<FacilityModel*>(new FakeSourceFacility());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SourceFacilityTest : public ::testing::Test {
  protected:
    FakeSourceFacility* src_facility;
    FakeSourceFacility* new_facility; 
    TestMarket* commod_market;

    virtual void SetUp(){
      src_facility = new FakeSourceFacility();
      src_facility->setParent(new TestInst());
      new_facility = new FakeSourceFacility();
      commod_market = new TestMarket(src_facility->getOutCommod());
    };

    virtual void TearDown() {
      delete src_facility;
      delete new_facility;
      delete commod_market;
    }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SourceFacilityTest, InitialState) {
  int time = 1;
  EXPECT_DOUBLE_EQ(0.0, src_facility->fakeCheckInventory());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SourceFacilityTest, CopyFacility) {
  new_facility->copy(src_facility); 
  EXPECT_DOUBLE_EQ(0.0, new_facility->fakeCheckInventory()); // fresh inventory
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SourceFacilityTest, CopyFreshModel) {
  new_facility->copyFreshModel(dynamic_cast<Model*>(src_facility)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<SourceFacility*>(new_facility)); // still a source facility
  EXPECT_NO_THROW(dynamic_cast<FakeSourceFacility*>(new_facility)); // still a fake source facility
  EXPECT_DOUBLE_EQ(0.0, new_facility->fakeCheckInventory()); // fresh inventory
  EXPECT_EQ(src_facility->getCapacity(), new_facility->getCapacity());
  EXPECT_EQ(src_facility->getInvSize(), new_facility->getInvSize());
  EXPECT_EQ(src_facility->getCommodPrice(), new_facility->getCommodPrice());
  EXPECT_DOUBLE_EQ(src_facility->getRecipe().mass(), new_facility->getRecipe().mass());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SourceFacilityTest, Print) {
  EXPECT_NO_THROW(src_facility->print());
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SourceFacilityTest, ReceiveMessage) {
  msg_ptr msg;
  EXPECT_THROW(src_facility->receiveMessage(msg), CycException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SourceFacilityTest, Tick) {
  int time = 1;
  EXPECT_DOUBLE_EQ(0.0, src_facility->fakeCheckInventory());
  EXPECT_NO_THROW(src_facility->handleTick(time));
  EXPECT_DOUBLE_EQ(0.0,src_facility->fakeCheckInventory());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SourceFacilityTest, Tock) {
  int time = 1;
  EXPECT_DOUBLE_EQ(0.0,src_facility->fakeCheckInventory());
  EXPECT_NO_THROW(src_facility->handleTock(time));
  EXPECT_GT(src_facility->fakeCheckInventory(),0.0);
  EXPECT_LE(src_facility->getCapacity(),src_facility->fakeCheckInventory());
  double expected_inventory = src_facility->getCapacity()*src_facility->getRecipe().mass(); 
  if (expected_inventory > src_facility->getInvSize()) {
    expected_inventory = src_facility->getInvSize();
  }
  EXPECT_DOUBLE_EQ(expected_inventory, src_facility->fakeCheckInventory());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(SourceFac, FacilityModelTests, Values(&SourceFacilityConstructor));
INSTANTIATE_TEST_CASE_P(SourceFac, ModelTests, Values(&SourceFacilityModelConstructor));

