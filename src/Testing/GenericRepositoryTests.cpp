#include <gtest/gtest.h>
#include <dlfcn.h>
#include "Utility/Timer.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class GenericRepositoryTest : public ::testing::Test {
  protected:
    string model_name, model_type;
    Model* TestModel1;
    Model* TestModel2;
    void* model;
    mdl_ctor* new_model;
    mdl_dtor* del_model;

    virtual void SetUp(){
      //test_empty_repo = new GenericRepository();
      model_name = "GenericRepository";
      model_type = "Facility";
      new_model = Model::load(model_type, model_name);
      TestModel1 = new_model();
      TestModel2 = new_model();
      TestModel1->setModelType("Facility");
      TestModel1->setModelType("Facility");
    };
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(GenericRepositoryTest, loadModel) {
  EXPECT_EQ(TestModel1->getModelType(), "Facility");
  EXPECT_NE(TestModel1->getSN(), NULL);
  EXPECT_NE(TestModel2->getSN(), NULL);
  EXPECT_NE(TestModel1->getSN(), TestModel2->getSN());
  // This sort of thing  will fail until we have a non xml model init funciton
  // EXPECT_EQ(TestModel1->getModelImpl(), "GenericRepository"); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(GenericRepositoryTest, SendMessage) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(GenericRepositoryTest, ReceiveMessage) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(GenericRepositoryTest, ReceiveMaterial) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(GenericRepositoryTest, EmplaceWaste) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(GenericRepositoryTest, LoadComponents) {
}
