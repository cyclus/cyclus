#include "position_test_facility.h"

#include <sstream>

#include "test_agents/injection_facilities/injectable_test.h"

namespace cyclus {
namespace toolkit {
namespace {

const double kConfiguredLatitude = 42.3601;
const double kConfiguredLongitude = -71.0589;
const double kDefaultInjectedLatitude = 0.0;
const double kDefaultInjectedLongitude = 0.0;
const double kPositionPrecision = 2e-5;

class PositionInjectionTest : public InjectableTest<PositionTestFacility> {
 protected:
  void ExpectValues(PositionTestFacility* facility) {
    EXPECT_DOUBLE_EQ(kConfiguredLatitude, facility->latitude);
    EXPECT_DOUBLE_EQ(kConfiguredLongitude, facility->longitude);
  }
};

std::string PositionConfig() {
  std::stringstream config;
  config << "<latitude>" << kConfiguredLatitude << "</latitude>"
         << "<longitude>" << kConfiguredLongitude << "</longitude>";
  return config.str();
}

TEST_F(PositionInjectionTest, AnnotationsAndDefaults) {
  PositionTestFacility* facility = Load("PositionTestFacility", "");
  Json::Value vars = facility->annotations()["vars"];

  ASSERT_TRUE(vars["latitude"].isObject());
  ASSERT_TRUE(vars["longitude"].isObject());
  EXPECT_DOUBLE_EQ(kDefaultInjectedLatitude,
                   vars["latitude"]["default"].asDouble());
  EXPECT_DOUBLE_EQ(kDefaultInjectedLongitude,
                   vars["longitude"]["default"].asDouble());
  EXPECT_DOUBLE_EQ(kDefaultInjectedLatitude, facility->latitude);
  EXPECT_DOUBLE_EQ(kDefaultInjectedLongitude, facility->longitude);
}

TEST_F(PositionInjectionTest, ExplicitInputCloneAndRestart) {
  PositionTestFacility* facility =
      Load("PositionTestFacility", PositionConfig());
  ExpectValues(facility);

  PositionTestFacility* clone =
      dynamic_cast<PositionTestFacility*>(facility->Clone());
  ASSERT_NE(nullptr, clone);
  ExpectValues(clone);

  QueryResult state = Snapshot(facility);
  ASSERT_EQ(1, state.rows.size());
  EXPECT_DOUBLE_EQ(kConfiguredLatitude, state.GetVal<double>("latitude"));
  EXPECT_DOUBLE_EQ(kConfiguredLongitude,
                   state.GetVal<double>("longitude"));

  PositionTestFacility* restored = Restore(facility);
  ExpectValues(restored);
}

TEST_F(PositionInjectionTest, EnterNotifyInitializesCoordinates) {
  PositionTestFacility* facility =
      Load("PositionTestFacility", PositionConfig());

  facility->EnterNotify();

  EXPECT_NEAR(kConfiguredLatitude, facility->coordinates.latitude(),
              kPositionPrecision);
  EXPECT_NEAR(kConfiguredLongitude, facility->coordinates.longitude(),
              kPositionPrecision);
}

}  // namespace
}  // namespace toolkit
}  // namespace cyclus
