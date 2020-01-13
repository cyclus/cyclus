#include <gtest/gtest.h>

#include "context.h"
#include "deploy_inst.h"
#include "institution_tests.h"
#include "agent_tests.h"

// make sure that the deployed agent's prototype name is identical to the
// originally specified prototype name - this is important to test because
// DeployInst does some mucking around with registering name-modded prototypes
// in order to deal with lifetime setting.

using cyclus::QueryResult;

TEST(DeployInstTests, ProtoNames) {
  std::string config =
     "<prototypes>  <val>foobar</val> </prototypes>"
     "<build_times> <val>1</val>      </build_times>"
     "<n_build>     <val>3</val>      </n_build>"
     "<lifetimes>   <val>2</val>      </lifetimes>"
     ;

  int simdur = 5;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:DeployInst"), config, simdur);
  sim.DummyProto("foobar");
  int id = sim.Run();

  cyclus::SqlStatement::Ptr stmt = sim.db().db().Prepare(
      "SELECT COUNT(*) FROM AgentEntry WHERE Prototype = 'foobar';"
      );
  stmt->Step();
  EXPECT_EQ(3, stmt->GetInt(0));
}

TEST(DeployInstTests, BuildTimes) {
  std::string config =
     "<prototypes>  <val>foobar</val> <val>foobar</val> </prototypes>"
     "<build_times> <val>1</val>      <val>3</val>      </build_times>"
     "<n_build>     <val>1</val>      <val>7</val>      </n_build>"
     ;

  int simdur = 5;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:DeployInst"), config, simdur);
  sim.DummyProto("foobar");
  int id = sim.Run();

  cyclus::SqlStatement::Ptr stmt = sim.db().db().Prepare(
      "SELECT COUNT(*) FROM AgentEntry WHERE Prototype = 'foobar' AND EnterTime = 1;"
      );
  stmt->Step();
  EXPECT_EQ(1, stmt->GetInt(0));

  stmt = sim.db().db().Prepare(
      "SELECT COUNT(*) FROM AgentEntry WHERE Prototype = 'foobar' AND EnterTime = 3;"
      );
  stmt->Step();
  EXPECT_EQ(7, stmt->GetInt(0));
}

// make sure that specified lifetimes are honored both in agent's table record
// and in decommissioning.
TEST(DeployInstTests, FiniteLifetimes) {
  std::string config =
     "<prototypes>  <val>foobar</val> <val>foobar</val> <val>foobar</val> </prototypes>"
     "<build_times> <val>1</val>      <val>1</val>      <val>2</val>      </build_times>"
     "<n_build>     <val>1</val>      <val>7</val>      <val>3</val>      </n_build>"
     "<lifetimes>   <val>1</val>      <val>2</val>      <val>-1</val>     </lifetimes>"
     ;

  int simdur = 5;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:DeployInst"), config, simdur);
  sim.DummyProto("foobar");
  int id = sim.Run();

  // check agent deployment
  cyclus::SqlStatement::Ptr stmt = sim.db().db().Prepare(
      "SELECT COUNT(*) FROM AgentEntry WHERE Prototype = 'foobar' AND EnterTime = 1 AND Lifetime = 1;"
      );
  stmt->Step();
  EXPECT_EQ(1, stmt->GetInt(0));

  stmt = sim.db().db().Prepare(
      "SELECT COUNT(*) FROM AgentEntry WHERE Prototype = 'foobar' AND EnterTime = 1 AND Lifetime = 2;"
      );
  stmt->Step();
  EXPECT_EQ(7, stmt->GetInt(0));

  stmt = sim.db().db().Prepare(
      "SELECT COUNT(*) FROM AgentEntry WHERE Prototype = 'foobar' AND EnterTime = 2 AND Lifetime = -1;"
      );
  stmt->Step();
  EXPECT_EQ(3, stmt->GetInt(0));

  // check decommissioning
  stmt = sim.db().db().Prepare(
      "SELECT COUNT(*) FROM AgentEntry As e JOIN AgentExit AS x ON x.AgentId = e.AgentId WHERE e.Prototype = 'foobar' AND x.ExitTime = 1;"
      );
  stmt->Step();
  EXPECT_EQ(1, stmt->GetInt(0));

  stmt = sim.db().db().Prepare(
      "SELECT COUNT(*) FROM AgentEntry As e JOIN AgentExit AS x ON x.AgentId = e.AgentId WHERE e.Prototype = 'foobar' AND x.ExitTime = 2;"
      );
  stmt->Step();
  EXPECT_EQ(7, stmt->GetInt(0));

  // agent with -1 lifetime should not be in AgentExit table
  stmt = sim.db().db().Prepare(
      "SELECT COUNT(*) FROM AgentExit;"
      );
  stmt->Step();
  EXPECT_EQ(8, stmt->GetInt(0));
}

TEST(DeployInstTests, NoDupProtos) {
  std::string config =
     "<prototypes>  <val>foobar</val> <val>foobar</val> <val>foobar</val> </prototypes>"
     "<build_times> <val>1</val>      <val>1</val>      <val>2</val>      </build_times>"
     "<n_build>     <val>1</val>      <val>7</val>      <val>3</val>      </n_build>"
     "<lifetimes>   <val>1</val>      <val>1</val>      <val>-1</val>     </lifetimes>"
     ;

  int simdur = 5;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:DeployInst"), config, simdur);
  sim.DummyProto("foobar");
  int id = sim.Run();

  // don't duplicate same prototypes with same custom lifetime
  cyclus::SqlStatement::Ptr stmt = sim.db().db().Prepare(
      "SELECT COUNT(*) FROM Prototypes WHERE Prototype = 'foobar_life_1';"
      );
  stmt->Step();
  EXPECT_EQ(1, stmt->GetInt(0));

  // don't duplicate custom lifetimes that are identical to original prototype
  // lifetime.
  stmt = sim.db().db().Prepare(
      "SELECT COUNT(*) FROM Prototypes WHERE Prototype = 'foobar';"
      );
  stmt->Step();
  EXPECT_EQ(1, stmt->GetInt(0));
}

TEST(DeployInstTests, PositionInitialize) {
  std::string config =
     "<prototypes>  <val>foobar</val> </prototypes>"
     "<build_times> <val>1</val>      </build_times>"
     "<n_build>     <val>3</val>      </n_build>"
     "<lifetimes>   <val>2</val>      </lifetimes>";

  int simdur = 5;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:DeployInst"), config, simdur);
  sim.DummyProto("foobar");
  int id = sim.Run();

  QueryResult qr = sim.db().Query("AgentPosition", NULL);
  EXPECT_EQ(qr.GetVal<double>("Latitude"), 0.0);
  EXPECT_EQ(qr.GetVal<double>("Longitude"), 0.0);
}

TEST(DeployInstTests, PositionInitialize2) {
  std::string config =
     "<prototypes>  <val>foobar</val> </prototypes>"
     "<longitude>   -20.0             </longitude>"
     "<latitude>    2.0               </latitude>"
     "<build_times> <val>1</val>      </build_times>"
     "<n_build>     <val>3</val>      </n_build>"
     "<lifetimes>   <val>2</val>      </lifetimes>";

  int simdur = 5;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:DeployInst"), config, simdur);
  sim.DummyProto("foobar");
  int id = sim.Run();

  QueryResult qr = sim.db().Query("AgentPosition", NULL);
  EXPECT_EQ(qr.GetVal<double>("Latitude"), 2.0);
  EXPECT_EQ(qr.GetVal<double>("Longitude"), -20.0);
}

// required to get functionality in cyclus agent unit tests library
cyclus::Agent* DeployInstitutionConstructor(cyclus::Context* ctx) {
  return new cycamore::DeployInst(ctx);
}

#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif  // CYCLUS_AGENT_TESTS_CONNECTED

INSTANTIATE_TEST_CASE_P(DeployInst, InstitutionTests,
                        Values(&DeployInstitutionConstructor));
INSTANTIATE_TEST_CASE_P(DeployInst, AgentTests,
                        Values(&DeployInstitutionConstructor));
