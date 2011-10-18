#include <gtest/gtest.h>
#include <dlfcn.h>
#include "Utility/Timer.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class GenericRepositoryTest : public ::testing::Test {
  protected:
    string model_name, model_type;
    Model* test_model_1;
    Model* test_model_2;
    void* model;
    mdl_ctor* new_model;
    mdl_dtor* del_model;

    virtual void SetUp(){
      model_name = "GenericRepository";
      model_type = "Facility";
      new_model = Model::loadConstructor(model_type, model_name);
      test_model_1 = new_model();
      test_model_2 = new_model();
      test_model_1->setModelType("Facility");
      test_model_1->setModelType("Facility");
    };
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(GenericRepositoryTest, loadModel) {
  EXPECT_EQ(test_model_1->getModelType(), "Facility");
  EXPECT_NE(test_model_1->getSN(), NULL);
  EXPECT_NE(test_model_2->getSN(), NULL);
  EXPECT_GT(test_model_1->getSN(),0);
  EXPECT_GT(test_model_2->getSN(), test_model_1->getSN());   
  // This sort of thing  will fail until we have a non xml model init funciton
  // EXPECT_EQ(test_model_1->getModelImpl(), "GenericRepository"); 
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
