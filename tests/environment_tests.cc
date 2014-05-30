#include "dynamic_module.h"
#include "env.h"

#include <fstream>
#include <string>

#include "boost/filesystem.hpp"
#include <gtest/gtest.h>

namespace fs = boost::filesystem;

using cyclus::Env;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(EnvironmentTests, CyclusPath) {
  // get the path before changing it
  std::string env_var = Env::GetEnv("CYCLUS_PATH");

  std::string path = "/my/nice/path";
  std::string cmd = "CYCLUS_PATH=" + path;
  putenv((char*) cmd.c_str());
  std::vector<std::string> ps = Env::cyclus_path();

  ASSERT_EQ(3, ps.size());
  EXPECT_EQ(path, ps[0]);

  // put the path back
  cmd = "CYCLUS_PATH=" + env_var;
  putenv(const_cast<char *>(cmd.c_str()));
  ASSERT_EQ(env_var, Env::GetEnv("CYCLUS_PATH"));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(EnvironmentTests, FindNonStandardPath) {
  // get the path before changing it
  std::string env_var = Env::GetEnv("CYCLUS_PATH");

  fs::path fname("libfoo.so");
  fs::path dir = fs::path(getenv("HOME")) / fs::path(".tmp-cyclus-test");
  fs::path full = dir / fname;

  // add file
  fs::create_directory(dir);
  std::ofstream f(full.string().c_str());
  f.close();

  // add path to env
  std::string cmd = "CYCLUS_PATH=" + dir.string();
  putenv((char*) cmd.c_str());

  fs::path p;
  ASSERT_NO_THROW(p = Env::FindModule(fname.string()));
  EXPECT_EQ(full, p);

  fs::remove_all(dir);

  // put the path back
  cmd = "CYCLUS_PATH=" + env_var;
  putenv(const_cast<char *>(cmd.c_str()));
  ASSERT_EQ(env_var, Env::GetEnv("CYCLUS_PATH"));
}
