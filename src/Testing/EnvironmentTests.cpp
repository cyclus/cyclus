
#include "Env.h"
#include "DynamicModule.h"

#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include "boost/filesystem.hpp"

using namespace std;
namespace fs = boost::filesystem;
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(EnvironmentTests, ModuleEnvVar) {
  string path = "/my/nice/path";
  string cmd = "CYCLUS_MODULE_PATH=" + path;
  putenv((char*)cmd.c_str());
  EXPECT_EQ(Env::moduleEnvVar(), path);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(EnvironmentTests, FindNonStandardPath) {
  fs::path fname("libfoo.so");
  fs::path dir = fs::path(Env::getInstallPath()) / fs::path("test_path");
  fs::path full = dir / fname;

  // add file
  fs::create_directory(dir);
  ofstream f(full.string().c_str());
  f.close();

  // add path to env
  string cmd = "CYCLUS_MODULE_PATH="+dir.string();
  putenv((char*)cmd.c_str());

  fs::path actual_path;
  ASSERT_TRUE(Env::findModuleLib(fname.string(), actual_path));
  EXPECT_EQ(full, actual_path);

  fs::remove_all(dir);
}

