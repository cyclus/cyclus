#include "unit_cost_test_facility.h"

#include <sstream>
#include <vector>

#include "test_agents/injection_facilities/injectable_test.h"

namespace cyclus {
namespace toolkit {
namespace {

const double kConfiguredUnitCost = -2.5;
const double kConfiguredUnitCostMod = -0.25;
const std::vector<double> kConfiguredUnitCostMods = {1.0, -2.0, 3.5};
const double kDefaultInjectedUnitCost = 1.0;
const double kDefaultInjectedUnitCostMod = 0.0;

class UnitCostTest : public InjectableTest<UnitCostTestFacility> {
 protected:
  void ExpectValues(UnitCostTestFacility* facility) {
    EXPECT_DOUBLE_EQ(kConfiguredUnitCost, facility->get_unit_cost());
    EXPECT_DOUBLE_EQ(kConfiguredUnitCostMod, facility->get_unit_cost_mod());
    EXPECT_EQ(kConfiguredUnitCostMods, facility->get_unit_cost_mods());
  }
};

std::string UnitCostConfig() {
  std::stringstream config;
  config << "<unit_cost>" << kConfiguredUnitCost << "</unit_cost>"
         << "<unit_cost_mod>" << kConfiguredUnitCostMod
         << "</unit_cost_mod><unit_cost_mods>";
  for (double modifier : kConfiguredUnitCostMods)
    config << "<val>" << modifier << "</val>";
  config << "</unit_cost_mods>";
  return config.str();
}

TEST_F(UnitCostTest, AnnotationsAndDefaults) {
  UnitCostTestFacility* facility = Load("UnitCostTestFacility", "");
  Json::Value vars = facility->annotations()["vars"];

  ASSERT_TRUE(vars["unit_cost"].isObject());
  ASSERT_TRUE(vars["unit_cost_mod"].isObject());
  ASSERT_TRUE(vars["unit_cost_mods"].isObject());
  EXPECT_DOUBLE_EQ(kDefaultInjectedUnitCost,
                 vars["unit_cost"]["default"].asDouble());
  EXPECT_DOUBLE_EQ(kDefaultInjectedUnitCostMod,
                 vars["unit_cost_mod"]["default"].asDouble());
  EXPECT_TRUE(vars["unit_cost_mods"]["default"].isArray());
  EXPECT_DOUBLE_EQ(kDefaultInjectedUnitCost, facility->get_unit_cost());
  EXPECT_DOUBLE_EQ(kDefaultInjectedUnitCostMod, facility->get_unit_cost_mod());
  EXPECT_TRUE(facility->get_unit_cost_mods().empty());
}

TEST_F(UnitCostTest, ExplicitInputCloneAndRestart) {
  UnitCostTestFacility* facility =
      Load("UnitCostTestFacility", UnitCostConfig());
  ExpectValues(facility);

  UnitCostTestFacility* clone =
      dynamic_cast<UnitCostTestFacility*>(facility->Clone());
  ASSERT_NE(nullptr, clone);
  ExpectValues(clone);

  QueryResult state = Snapshot(facility);
  ASSERT_EQ(1, state.rows.size());
  EXPECT_DOUBLE_EQ(kConfiguredUnitCost, state.GetVal<double>("unit_cost"));
  EXPECT_DOUBLE_EQ(kConfiguredUnitCostMod,
                   state.GetVal<double>("unit_cost_mod"));
  EXPECT_EQ(kConfiguredUnitCostMods,
            state.GetVal<std::vector<double> >("unit_cost_mods"));

  UnitCostTestFacility* restored = Restore(facility);
  ExpectValues(restored);
}

TEST_F(UnitCostTest, GettersAndSetters) {
  UnitCostTestFacility* facility = Load("UnitCostTestFacility", "");

  double new_cost = -4.0;
  double new_cost_mod = 1.5;
  std::vector<double> new_cost_mods = {2.0, 3.0};

  facility->set_unit_cost(new_cost);
  facility->set_unit_cost_mod(new_cost_mod);
  facility->set_unit_cost_mods(new_cost_mods);

  EXPECT_DOUBLE_EQ(new_cost, facility->get_unit_cost());
  EXPECT_DOUBLE_EQ(new_cost_mod, facility->get_unit_cost_mod());
  EXPECT_EQ(new_cost_mods, facility->get_unit_cost_mods());
}

}  // namespace
}  // namespace toolkit
}  // namespace cyclus
