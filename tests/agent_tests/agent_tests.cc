#include "agent_tests.h"

#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "infile_tree.h"
#include "pyhooks.h"
#include "xml_parser.h"

/// this function should be called by all most derived agents in order to get
/// access to the functionality of the agent unit test library
extern int ConnectAgentTests() { return 0; }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_P(AgentTests, Clone) {
  cyclus::PyStart();
  cyclus::Agent* clone = agent_->Clone();
  delete clone;
  cyclus::PyStop();
}

TEST_P(AgentTests, Print) {
  std::string s = agent_->str();
  EXPECT_NO_THROW(std::string s = agent_->str());
}

TEST_P(AgentTests, Schema) {
  std::stringstream schema;
  schema << "<grammar xmlns:a=\"http://relaxng.org/ns/annotation/1.0\">\n";
  schema << "<element name=\"foo\">\n";
  schema << agent_->schema();
  schema << "</element>\n</grammar>\n";
  std::cout << schema.str();
  cyclus::XMLParser p;
  EXPECT_NO_THROW(p.Init(schema));
}

TEST_P(AgentTests, GetAgentType) {
  EXPECT_NE(std::string("Agent"), agent_->kind());
}

TEST_P(AgentTests, Annotations) {
  Json::Value anno = agent_->annotations();
  EXPECT_TRUE(anno.isObject());
}

TEST_P(AgentTests, Annotations_Name) {
  Json::Value a = agent_->annotations();
  EXPECT_TRUE(a["name"].isString());
}

TEST_P(AgentTests, Annotations_Parents) {
  Json::Value a = agent_->annotations();
  EXPECT_FALSE(a["parents"].empty());
}

TEST_P(AgentTests, Annotations_AllParents) {
  Json::Value all_parents = agent_->annotations()["all_parents"];
  EXPECT_FALSE(all_parents.empty());

  Json::Value agent = Json::Value("cyclus::Agent");
  bool isagent = false;
  for (int i = 0; i < all_parents.size(); ++i)
    if (agent == all_parents[i])
      isagent = true;
  EXPECT_TRUE(isagent);
}

// EconomicEntity Tests
TEST_P(AgentTests, EmptyGetEconParam) {
  EXPECT_THROW(agent_->GetEconParameter("UnitTestHook"), std::runtime_error);
}

TEST_P(AgentTests, SetThenGetEconParam) {
  agent_->SetEconParameter("UnitTestHook", -1.0);
  double data = agent_->GetEconParameter("UnitTestHook");
  EXPECT_EQ(data, -1.0);
}

TEST_P(AgentTests, PV_A_and_F) {
  double present_value = agent_->PV(10, 0.05, 10000.0, 100.0);
  double pv_hand_calc = 6911.306028;
  EXPECT_NEAR(present_value, pv_hand_calc, 1e-3);
}

TEST_P(AgentTests, PV_only_F) {
  double present_value = agent_->PV(10, 0.05, 10000.0, 0.0);
  double pv_hand_calc = 6139.132535;
  EXPECT_NEAR(present_value, pv_hand_calc, 1e-3);
}

TEST_P(AgentTests, PV_only_A) {
  double present_value = agent_->PV(10, 0.05, 0.0, 100.0);
  double pv_hand_calc = 772.1734929;
  EXPECT_NEAR(present_value, pv_hand_calc, 1e-3);
}

TEST_P(AgentTests, PV_i_equals_zero) {
  double present_value = agent_->PV(10, 0.0, 10000.0, 100.0);
  double pv_hand_calc = 11000.0;
  EXPECT_NEAR(present_value, pv_hand_calc, 1e-3);
}

TEST_P(AgentTests, PV_n_lessthan_zero) {
  EXPECT_THROW(agent_->PV(-10, 0.0, 10000.0, 100.0), std::invalid_argument);
}

TEST_P(AgentTests, PV_n_equals_zero) {
  EXPECT_THROW(agent_->PV(0, 0.0, 10000.0, 100.0), std::invalid_argument);
}

TEST_P(AgentTests, FV_A_and_P) {
  double future_value = agent_->FV(10, 0.05, 10000.0, 100.0);
  double fv_hand_calc = 17546.73552;
  EXPECT_NEAR(future_value, fv_hand_calc, 1e-3);
}

TEST_P(AgentTests, FV_only_P) {
  double future_value = agent_->FV(10, 0.05, 10000.0, 0.0);
  double fv_hand_calc = 16288.946267;
  EXPECT_NEAR(future_value, fv_hand_calc, 1e-3);
}

TEST_P(AgentTests, FV_only_A) {
  double future_value = agent_->FV(10, 0.05, 0.0, 100.0);
  double fv_hand_calc = 1257.7892535;
  EXPECT_NEAR(future_value, fv_hand_calc, 1e-3);
}

TEST_P(AgentTests, FV_i_equals_zero) {
  double future_value = agent_->FV(10, 0.0, 10000.0, 100.0);
  double fv_hand_calc = 11000.0;
  EXPECT_NEAR(future_value, fv_hand_calc, 1e-3);
}

TEST_P(AgentTests, FV_n_lessthan_zero) {
  EXPECT_THROW(agent_->FV(-10, 0.0, 10000.0, 100.0), std::invalid_argument);
}

TEST_P(AgentTests, FV_n_equals_zero) {
  EXPECT_THROW(agent_->FV(0, 0.0, 10000.0, 100.0), std::invalid_argument);
}

TEST_P(AgentTests, PMT) {
  double payment = agent_->PMT(10, 0.05, 10000.0, 40000.0);
  double pmt_hand_calc = 4475.2287;
  EXPECT_NEAR(payment, pmt_hand_calc, 1e-3);
}

TEST_P(AgentTests, PMT_against_PV_and_FV) {
  double present_value = agent_->PV(10, 0.05, 0.0, agent_->PMT(10, 0.05, 1.0, 0.0));
  EXPECT_NEAR(present_value, 1.0, 1e-3);
}

TEST_P(AgentTests, PV_def_by_A) {
  std::vector<double> payments = {100.0, 200.0, 300.0, 10.0, 15.0, 1000.0};
  double pv = agent_->PV(0.05, payments);
  double pv_hand_calc = 1301.990584;
  EXPECT_NEAR(pv, pv_hand_calc, 1e-3);
}

TEST_P(AgentTests, PV_against_PV) {
  std::vector<double> payments = {100.0, 100.0, 100.0, 100.0, 100.0, 100.0};
  double pv_vec = agent_->PV(0.05, payments);
  double pv_regular = agent_->PV(6, 0.05, 0.0, 100.0);
  EXPECT_NEAR(pv_vec, pv_regular, 1e-3);
}


