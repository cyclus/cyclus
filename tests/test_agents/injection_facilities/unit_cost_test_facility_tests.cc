#include "unit_cost_test_facility.h"

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

#include "infile_tree.h"
#include "pyhooks.h"
#include "sqlite_back.h"
#include "test_context.h"
#include "xml_parser.h"

namespace cyclus {
namespace toolkit {
namespace {

class UnitCostTest : public ::testing::Test {
 protected:
  UnitCostTest() : back_(":memory:"), ctx_(&timer_, &rec_), snapshot_time_(0) {}

  virtual void SetUp() {
    PyStart();
    rec_.RegisterBackend(&back_);
    ctx_.InitSim(SimInfo(1));
  }

  virtual void TearDown() { PyStop(); }

  UnitCostTestFacility* Load(const std::string& config) {
    ctx_.time(0);
    UnitCostTestFacility* facility = new UnitCostTestFacility(&ctx_);

    std::stringstream xml;
    xml << "<facility><name>unit-cost</name><config>"
        << "<UnitCostTestFacility>" << config
        << "</UnitCostTestFacility></config></facility>";
    XMLParser parser;
    parser.Init(xml);
    InfileTree tree(parser);
    facility->Agent::InfileToDb(&tree, DbInit(facility, true));
    facility->InfileToDb(&tree, DbInit(facility));
    rec_.Flush();

    std::vector<Cond> conds;
    conds.push_back(Cond("AgentId", "==", facility->id()));
    conds.push_back(Cond("SimTime", "==", 0));
    CondInjector filtered(&back_, conds);
    PrefixInjector base(&filtered, "AgentState");
    PrefixInjector state(&filtered, StateTable(facility));
    facility->Agent::InitFrom(&base);
    facility->InitFrom(&state);
    return facility;
  }

  QueryResult Snapshot(UnitCostTestFacility* facility) {
    ctx_.time(++snapshot_time_);
    facility->Agent::Snapshot(DbInit(facility, true));
    facility->Snapshot(DbInit(facility));
    rec_.Flush();
    std::vector<Cond> conds = Conditions(facility);
    return back_.Query(StateTable(facility) + "Info", &conds);
  }

  UnitCostTestFacility* Restore(UnitCostTestFacility* facility) {
    std::vector<Cond> conds = Conditions(facility);
    CondInjector filtered(&back_, conds);
    PrefixInjector base(&filtered, "AgentState");
    PrefixInjector state(&filtered, StateTable(facility));
    UnitCostTestFacility* restored = new UnitCostTestFacility(&ctx_);
    restored->Agent::InitFrom(&base);
    restored->InitFrom(&state);
    return restored;
  }

  std::string StateTable(UnitCostTestFacility* facility) const {
    return "AgentState" + AgentSpec(facility->spec()).Sanitize();
  }

  std::vector<Cond> Conditions(UnitCostTestFacility* facility) const {
    std::vector<Cond> conds;
    conds.push_back(Cond("AgentId", "==", facility->id()));
    conds.push_back(Cond("SimTime", "==", snapshot_time_));
    return conds;
  }

  void ExpectValues(UnitCostTestFacility* facility) {
    EXPECT_DOUBLE_EQ(-2.5, facility->get_unit_cost());
    EXPECT_DOUBLE_EQ(-0.25, facility->get_unit_cost_mod());
    const std::vector<double> expected = {1.0, -2.0, 3.5};
    EXPECT_EQ(expected, facility->get_unit_cost_mods());
  }

  SqliteBack back_;
  Recorder rec_;
  Timer timer_;
  FakeContext ctx_;
  int snapshot_time_;
};

const char* const kConfig =
    "<unit_cost>-2.5</unit_cost>"
    "<unit_cost_mod>-0.25</unit_cost_mod>"
    "<unit_cost_mods><val>1.0</val><val>-2.0</val><val>3.5</val>"
    "</unit_cost_mods>";

TEST_F(UnitCostTest, AnnotationsAndDefaults) {
  UnitCostTestFacility* facility = Load("");
  Json::Value vars = facility->annotations()["vars"];

  ASSERT_TRUE(vars["unit_cost"].isObject());
  ASSERT_TRUE(vars["unit_cost_mod"].isObject());
  ASSERT_TRUE(vars["unit_cost_mods"].isObject());
  EXPECT_DOUBLE_EQ(0.0, vars["unit_cost"]["default"].asDouble());
  EXPECT_DOUBLE_EQ(0.0, vars["unit_cost_mod"]["default"].asDouble());
  EXPECT_TRUE(vars["unit_cost_mods"]["default"].isArray());
  EXPECT_DOUBLE_EQ(0.0, facility->get_unit_cost());
  EXPECT_DOUBLE_EQ(0.0, facility->get_unit_cost_mod());
  EXPECT_TRUE(facility->get_unit_cost_mods().empty());
}

TEST_F(UnitCostTest, ExplicitInputCloneAndRestart) {
  UnitCostTestFacility* facility = Load(kConfig);
  ExpectValues(facility);

  UnitCostTestFacility* clone =
      dynamic_cast<UnitCostTestFacility*>(facility->Clone());
  ASSERT_NE(nullptr, clone);
  ExpectValues(clone);

  QueryResult state = Snapshot(facility);
  ASSERT_EQ(1, state.rows.size());
  EXPECT_DOUBLE_EQ(-2.5, state.GetVal<double>("unit_cost"));
  EXPECT_DOUBLE_EQ(-0.25, state.GetVal<double>("unit_cost_mod"));
  EXPECT_EQ((std::vector<double>{1.0, -2.0, 3.5}),
            state.GetVal<std::vector<double> >("unit_cost_mods"));

  UnitCostTestFacility* restored = Restore(facility);
  ExpectValues(restored);
}

TEST_F(UnitCostTest, GettersAndSetters) {
  UnitCostTestFacility* facility = Load("");
  facility->set_unit_cost(-4.0);
  facility->set_unit_cost_mod(1.5);
  facility->set_unit_cost_mods(std::vector<double>{2.0, 3.0});

  EXPECT_DOUBLE_EQ(-4.0, facility->get_unit_cost());
  EXPECT_DOUBLE_EQ(1.5, facility->get_unit_cost_mod());
  EXPECT_EQ((std::vector<double>{2.0, 3.0}), facility->get_unit_cost_mods());
}

}  // namespace
}  // namespace toolkit
}  // namespace cyclus
