
#include "Env.h"
#include "DynamicModule.h"

#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include "boost/filesystem.hpp"

namespace fs = boost::filesystem;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(EnvironmentTests, ModuleEnvVar) {
  std::string path = "/my/nice/path";
  std::string cmd = cyclus::Env::ModuleEnvVarName() + '=' + path;
  putenv((char*) cmd.c_str());
  EXPECT_EQ(cyclus::Env::ModuleEnvVar(), path);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(EnvironmentTests, FindNonStandardPath) {
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
}

