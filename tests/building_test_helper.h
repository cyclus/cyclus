#ifndef CYCLUS_TESTS_BUILD_TEST_HELPER_H_
#define CYCLUS_TESTS_BUILD_TEST_HELPER_H_

#include <string>

#include "builder.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class BuildingTestHelper {
 public:
  /// constructor
  BuildingTestHelper();

  /// destructor
  ~BuildingTestHelper();

  /// first builder
  cyclus::Builder* builder1;

  /// second builder
  cyclus::Builder* builder2;
};

#endif  // CYCLUS_TESTS_BUILD_TEST_HELPER_H_
