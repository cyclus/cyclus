#include "building_test_helper.h"

namespace cyclus {
namespace toolkit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BuildingTestHelper::BuildingTestHelper() {
  builder1 = new Builder();
  builder2 = new Builder();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BuildingTestHelper::~BuildingTestHelper() {
  delete builder1;
  delete builder2;
}

} // namespace toolkit
} // namespace cyclus
