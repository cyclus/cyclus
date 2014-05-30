#ifndef CYCLUS_TESTS_BUILDING_TEST_HELPER_H_
#define CYCLUS_TESTS_BUILDING_TEST_HELPER_H_

#include <string>

#include "toolkit/builder.h"

namespace cyclus {
namespace toolkit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class BuildingTestHelper {
 public:
  /// constructor
  BuildingTestHelper();

  /// destructor
  ~BuildingTestHelper();

  /// first builder
  Builder* builder1;

  /// second builder
  Builder* builder2;
};

} // namespace toolkit
} // namespace cyclus

#endif  // CYCLUS_TESTS_BUILDING_TEST_HELPER_H_
