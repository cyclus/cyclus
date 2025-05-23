#include "platform.h"
#include <iostream>
#include <string>
#include <stdio.h>

#include <gtest/gtest.h>
#if CYCLUS_IS_PARALLEL
#include <omp.h>
#endif // CYCLUS_IS_PARALLEL
#include "env.h"
#include "logger.h"

int main(int argc, char* argv[]) {
  // Tell ENV the path between the cwd and the cyclus executable
  using cyclus::Env;
  using cyclus::Logger;
  std::string path = Env::PathBase(argv[0]);
  Logger::ReportLevel() = cyclus::LEV_ERROR;

  // Add the build path to the environment for testing
  std::string test_env = "CYCLUS_PATH=" + Env::GetBuildPath();
  std::string curr_var = Env::GetEnv("CYCLUS_PATH");
  if (curr_var != "") {
    test_env += ":" + curr_var;
  }
  putenv(const_cast<char *>(test_env.c_str()));

  #if CYCLUS_IS_PARALLEL
  omp_set_num_threads(1);
  #endif // CYCLUS_IS_PARALLEL

  for ( int i = 0; i < argc; i++ ) {
    std::string arg = argv[i];
    if ( arg == "--help" ) {
      std::cout << "GTest flags" << std::endl;
      std::cout << "\t--gtest_list_tests List the tests" << std::endl;
      std::cout << "\t--gtest_repeat Number of times to repeat each test" << std::endl;
      std::cout << "\t--gtest_filter Glob filter of test name" << std::endl;
      std::cout << "\t--gtest_print_time Time required to run" << std::endl;
      std::cout << "\nBy default, a Google Test program runs all tests the user has defined. Sometimes, you want to run only a subset of the tests (e.g. for debugging or quickly verifying a change). If you set the GTEST_FILTER environment variable or the --gtest_filter flag to a filter string, Google Test will only run the tests whose full names (in the form of TestCaseName.TestName) match the filter.\n"
        "The format of a filter is a ':'-separated list of wildcard patterns (called the positive patterns) optionally followed by a '-' and another ':'-separated pattern list (called the negative patterns). A test matches the filter if and only if it matches any of the positive patterns but does not match any of the negative patterns.\n"
        "A pattern may contain '*' (matches any string) or '?' (matches any single character). For convenience, the filter '*-NegativePatterns' can be also written as '-NegativePatterns'.\n"
        "For example:\n\n"
        " * ./foo_test Has no flag, and thus runs all its tests.\n"
        " * ./foo_test --gtest_filter=* Also runs everything, due to the single match-everything * value.\n"
        " * ./foo_test --gtest_filter=FooTest.* Runs everything in test case FooTest.\n"
        " * ./foo_test --gtest_filter=*Null*:*Constructor* Runs any test whose full name contains either \"Null\" or \"Constructor\".\n"
        " * ./foo_test --gtest_filter=-*DeathTest.* Runs all non-death tests.\n"
        " * ./foo_test --gtest_filter=FooTest.*-FooTest.Bar Runs everything in test case FooTest except FooTest.Bar. " << std::endl;
      return 0;
    }
  }
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
