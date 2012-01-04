// RecipeReactorTests.cpp
#include <gtest/gtest.h>

#include "RecipeReactor.h"
#include "CycException.h"
#include "Message.h"
#include "FacilityModelTests.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeRecipeReactor : public RecipeReactor {
  public:
    FakeRecipeReactor() : RecipeReactor() {

      // set the current month in cycle to 1, it's the first month.
      month_in_cycle_ = 1;
      cycle_time_ = 3;

      CompMap test_comp;

      Iso u235 = 92235;
      Atoms one = 1.0;
      test_comp[u235]=one;
      string test_mat_unit = "test_mat_unit";
      string test_rec_name = "test_rec_name";
      double test_size = 10.0;
      Basis test_type = ATOMBASED;
      bool test_template = true;

      Material* recipe = new Material(test_comp, test_mat_unit, test_rec_name, test_size, 
          test_type, test_template); 

      capacity_ = 2;
      inventory_size_ = 50;

      // initialize ordinary objects
      lifetime_ = 10;
      startConstrYr_ = 2010;
      startConstrMo_ = 1;
      startOpYr_ = 2010;
      startOpMo_ = 2;
      licExpYr_ = 2070;
      licExpMo_ = 1;
      state_ = "tx";
      typeReac_ = "PWR"; 
      CF_ = .9;

      // all facilities require commodities - possibly many
      string recipe_name;
      std::string in_commod;
      std::string out_commod;
      Material* in_recipe;
      Material* out_recipe;

      // for each fuel pair, there is an in and an out commodity
      int pairs = 1;
      for (int i = 0; i < pairs; i++){
        // get commods
        out_commod = "out-commod";
        in_commod = "in-commod";

        // get in_recipe
        in_recipe = recipe;

        // get out_recipe
        out_recipe = recipe; 

        fuelPairs_.push_back(make_pair(make_pair(in_commod,in_recipe),
              make_pair(out_commod, out_recipe)));
      };

      stocks_ = deque<InFuel>();
      currCore_ = deque< pair<std::string, Material* > >();
      inventory_ = deque< pair<std::string, Material*> >();
      ordersWaiting_ = deque< Message*>();

    }

    virtual ~FakeRecipeReactor() {
    }

    double fakeCheckInventory() { return checkInventory(); }
    double getCapacity() {return capacity_;}
    double getInvSize() {return inventory_size_;}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel* RecipeReactorConstructor(){
  return dynamic_cast<FacilityModel*>(new FakeRecipeReactor());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class RecipeReactorTest : public ::testing::Test {
  protected:
    FakeRecipeReactor* src_facility;
    FakeRecipeReactor* new_facility; 

    virtual void SetUp(){
      src_facility = new FakeRecipeReactor();
      src_facility->setParent(new TestInst());
      new_facility = new FakeRecipeReactor();
      // for facilities that trade commodities, create appropriate markets here
    };

    virtual void TearDown() {
      delete src_facility;
      // for facilities that trade commodities, delete appropriate markets here
    }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(RecipeReactorTest, InitialState) {
  // Test things about the initial state of the facility here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(RecipeReactorTest, CopyFreshModel) {
  new_facility->copyFreshModel(dynamic_cast<Model*>(src_facility)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<RecipeReactor*>(new_facility)); // still a recipe reactor
  EXPECT_NO_THROW(dynamic_cast<FakeRecipeReactor*>(new_facility)); // still a fake recipe reactor
  // Test that RecipeReactor specific parameters are initialized in the deep copy method here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(RecipeReactorTest, Print) {
  EXPECT_NO_THROW(src_facility->print());
  // Test RecipeReactor specific aspects of the print method here
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(RecipeReactorTest, ReceiveMessage) {
  Message* msg;
  // Test RecipeReactor specific behaviors of the receiveMessage function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(RecipeReactorTest, Tick) {
  int time = 1;
  EXPECT_NO_THROW(src_facility->handleTick(time));
  // Test RecipeReactor specific behaviors of the handleTick function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(RecipeReactorTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_facility->handleTick(time));
  // Test RecipeReactor specific behaviors of the handleTock function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(RecipeReactor, FacilityModelTests, Values(&RecipeReactorConstructor));

