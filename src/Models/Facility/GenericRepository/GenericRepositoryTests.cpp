// GenericRepositoryTests.cpp
#include <gtest/gtest.h>
#include <dlfcn.h>

#include "GenericRepository.h"
#include "Utility/Timer.h"
#include "ModelTests.h"
#include "FacilityModelTests.h"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeGenericRepository : public GenericRepository {
  public:
    FakeGenericRepository() : GenericRepository() {

      // initialize ordinary objects
      x_ = 10;
      y_ = 10;
      z_ = 10;
      dx_ = 1;
      dy_ = 1;
      dz_ = 1;
      capacity_ = 100;
      inventory_size_ = 70000;
      lifetime_ = 3000000;
      start_op_yr_ = 1; 
      start_op_mo_ = 1;

      // The repository accepts any commodities designated waste.
      // This will be a list

      /// all facilities require commodities - possibly many
      in_commods_.push_back("in-commod");

      // get components
      Component* new_comp;

      // initialize things that don't depend on the input
      stocks_ = deque< WasteStream >();
      inventory_ = deque< WasteStream >();
      waste_packages_ = deque< Component* >();
      waste_forms_ = deque< Component* >();
      is_full_ = false;
    }

    virtual ~FakeGenericRepository() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class GenericRepositoryTest : public ::testing::Test {
  protected:
    string model_name, model_type;
    Model* test_model_1;
    Model* test_model_2;
    void* model;
    mdl_ctor* new_model;
    mdl_dtor* del_model;
    FakeGenericRepository* src_facility;

    virtual void SetUp(){
      src_facility = new FakeGenericRepository();
      src_facility->setParent(new TestInst());
      model_name = "GenericRepository";
      model_type = "Facility";
      new_model = Model::loadConstructor(model_type, model_name);
      test_model_1 = new_model();
      test_model_2 = new_model();
      test_model_1->setModelType("Facility");
      test_model_1->setModelType("Facility");
    };
    virtual void TearDown() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* GenericRepositoryModelConstructor(){
  return dynamic_cast<Model*>(new FakeGenericRepository());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel* GenericRepositoryFacilityConstructor(){
  return dynamic_cast<FacilityModel*>(new FakeGenericRepository());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(GenericRepositoryTest, loadModel) {
  EXPECT_EQ(test_model_1->getModelType(), "Facility");
  EXPECT_NE(test_model_1->ID(), NULL);
  EXPECT_NE(test_model_2->ID(), NULL);
  EXPECT_GT(test_model_1->ID(),0);
  EXPECT_GT(test_model_2->ID(), test_model_1->ID());   
  // This sort of thing  will fail until we have a non xml model init funciton
  // EXPECT_EQ(test_model_1->getModelImpl(), "GenericRepository"); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//INSTANTIATE_TEST_CASE_P(GenericRepositoryFac, FacilityModelTests, Values(&GenericRepositoryFacilityConstructor));
//INSTANTIATE_TEST_CASE_P(GenericRepositoryFac, ModelTests, Values(&GenericRepositoryModelConstructor));

