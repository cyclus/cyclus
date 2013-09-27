
#include "env.h"
#include "dynamic_module.h"

#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include "boost/filesystem.hpp"

namespace fs = boost::filesystem;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(EnvironmentTests, ModuleEnvVar) {
  // this works if the module path is added in the test driver app
  //EXPECT_EQ(cyclus::Env::ModuleEnvVar(), cyclus::Env::GetBuildPath());
  
  // get the path before changing it
  std::string env_var = cyclus::Env::ModuleEnvVar();
  
  std::string path = "/my/nice/path";
  std::string cmd = cyclus::Env::ModuleEnvVarName() + '=' + path;
  putenv((char*) cmd.c_str());
  EXPECT_EQ(cyclus::Env::ModuleEnvVar(), path);

  // put the path back
  cmd = cyclus::Env::ModuleEnvVarName() + '=' + env_var;
  putenv(const_cast<char *>(cmd.c_str()));
  ASSERT_EQ(cyclus::Env::ModuleEnvVar(), env_var);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(EnvironmentTests, FindNonStandardPath) {
  // get the path before changing it
  std::string env_var = cyclus::Env::ModuleEnvVar();
  
  fs::path fname("libfoo.so");
  fs::path dir = fs::path(getenv("HOME")) / fs::path(".tmp-cyclus-test");
  fs::path full = dir / fname;

  // add file
  fs::create_directory(dir);
  std::ofstream f(full.string().c_str());
  f.close();

  // add path to env
  std::string cmd = cyclus::Env::ModuleEnvVarName() + '=' + dir.string();
  putenv((char*) cmd.c_str());

  fs::path actual_path;
  ASSERT_TRUE(cyclus::Env::FindModuleLib(fname.string(), actual_path));
  EXPECT_EQ(full, actual_path);

  fs::remove_all(dir);

  // put the path back
  cmd = cyclus::Env::ModuleEnvVarName() + '=' + env_var;
  putenv(const_cast<char *>(cmd.c_str()));
  ASSERT_EQ(cyclus::Env::ModuleEnvVar(), env_var);
}

