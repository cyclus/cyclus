#include <gtest/gtest.h>

#include "Env.h"
#include "Model.h"
#include "Prototype.h"
#include "DynamicModule.h"

//#include <cstdlib>
#include <stdlib.h>
#include <fstream>
#include "boost/filesystem.hpp"

using namespace std;
namespace fs = boost::filesystem;

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
  EXPECT_NO_THROW(Model::loadModule("Facility","TestFacility");
                  Model::unloadModules(););
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(DynamicLoadingTests,FindNonStandardPath) 
{
  // set up
  string name = "otherfac";
  string lib_name = "lib" + name + DynamicModule::suffix();
  stringstream path;
  path << Env::getInstallPath() << Env::pathDelimeter() << 
    "test_path" << Env::pathDelimeter();
  fs::create_directory(fs::path(path.str()));
  string abs_path = path.str()+lib_name;
  // add file
  ofstream f(abs_path.c_str());
  f.close();
  // add path to env
  string cmd = Env::moduleEnvVar()+"="+path.str();
  putenv((char*)cmd.c_str());
  // test
  DynamicModule mod = DynamicModule("Facility",name);
  cout << getenv(Env::moduleEnvVar().c_str()) << endl;
  EXPECT_NO_THROW(mod.setPath());
  EXPECT_STREQ(abs_path.c_str(),mod.path().c_str());
  // clean up
  fs::remove_all(fs::path(path.str()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(DynamicLoadingTests,LoadLibError) 
{
  DynamicModule mod = DynamicModule("Facility","not_a_fac");
  EXPECT_THROW(mod.setPath(),CycIOException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(DynamicLoadingTests,ConstructTestFacility) 
{
  EXPECT_NO_THROW(Model::loadModule("Facility","TestFacility");
                  Model* fac = Model::constructModel("TestFacility");
                  Model::deleteModel(fac);
                  Model::unloadModules(););
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(DynamicLoadingTests,cloneTestFacility) 
{
  EXPECT_NO_THROW(Model::loadModule("Facility","TestFacility");
                  Model* fac = Model::constructModel("TestFacility");
                  Prototype* clone = dynamic_cast<Prototype*>(fac)->clone();
                  Model::deleteModel(dynamic_cast<Model*>(clone));
                  Model::deleteModel(fac);
                  Model::unloadModules(););
}
