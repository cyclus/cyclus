#ifndef CYCLUS_TESTS_TEST_AGENTS_TEST_POSITIONFAC_H_
#define CYCLUS_TESTS_TEST_AGENTS_TEST_POSITIONFAC_H_

#include <sstream>
#include "cyclus.h"
#include "toolkit/position.h"

namespace cyclus {
/// This is the simplest possible PositionFac, for testing
class TestPositionFac : public cyclus::toolkit::Position {
 public:
  TestPositionFac(double latitude, double longitude)
      : cyclus::toolkit::Position(0, 0) {}
  virtual ~TestPositionFac() {}
  // virtual cyclus::Agent* Clone() { return new TestPositionFac(context()); }
};

}  // namespace cyclus

#endif  // CYCLUS_TESTS_TEST_AGENTS_TEST_POSITIONFAC_H_
