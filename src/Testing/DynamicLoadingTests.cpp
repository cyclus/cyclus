#include <gtest/gtest.h>

#include "Model.h"
#include "Prototype.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
class DynamicLoadingTests : public ::testing::Test 
{
public:  
  virtual void SetUp()
  {
  }

  virtual void TearDown()
  {
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(DynamicLoadingTests,LoadTestFacility) 
{
  Model::loadModule("Facility","TestFacility");
  Model::unloadModules();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(DynamicLoadingTests,ConstructTestFacility) 
{
  Model::loadModule("Facility","TestFacility");
  Model* fac = Model::constructModel("TestFacility");
  Model::deleteModel(fac);
  Model::unloadModules();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(DynamicLoadingTests,cloneTestFacility) 
{
  Model::loadModule("Facility","TestFacility");
  Model* fac = Model::constructModel("TestFacility");
  Prototype* clone = dynamic_cast<Prototype*>(fac)->clone();
  Model::deleteModel(dynamic_cast<Model*>(clone));
  Model::deleteModel(fac);
  Model::unloadModules();
}
