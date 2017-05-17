#ifndef CYCLUS_TESTS_TEST_AGENTS_TEST_GISFAC_H_
#define CYCLUS_TESTS_TEST_AGENTS_TEST_GISFAC_H_

#include <sstream>
#include "cyclus.h"
#include "toolkit/gis.h"

namespace cyclus {
/// This is the simplest possible GISFac, for testing
class TestGISFac : public cyclus::toolkit::GIS {
 public:
  TestGISFac(long latitude, long longitude) : cyclus::toolkit::GIS(0, 0) {}
  virtual ~TestGISFac() {}
  // virtual cyclus::Agent* Clone() { return new TestGISFac(context()); }
};

}  // namespace cyclus

#endif  // CYCLUS_TESTS_TEST_AGENTS_TEST_GISFAC_H_