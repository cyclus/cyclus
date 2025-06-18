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

TEST_P(AgentTests, PV) {

  int n = 5;
  double i = 0.1;
  double F = 1;
  double A = 0.5;
  double finance_eps = 1e-4;

  EXPECT_NEAR(F, agent_->PV(n,0,F,0), finance_eps);  // discount rate = 0
  EXPECT_NEAR(F, agent_->PV(0,i,F,0), finance_eps);  // discount time = 0
  EXPECT_NEAR(F / (1+i), agent_->PV(1,i,F,0), finance_eps); // discount time = 1
  EXPECT_NEAR(F / std::pow((1+i), n), agent_->PV(n,i,F,0), finance_eps); // general
  EXPECT_NEAR(F / std::pow((1 + (i/12)), n*12), agent_->PV(n*12,i/12,F,0), finance_eps); // general
  EXPECT_NEAR(F / std::pow((1+i), -n), agent_->PV(-n,i,F,0), finance_eps); // negative time

  EXPECT_NEAR(A, agent_->PV(n,0,0,A), finance_eps); // discount rate = 0
  EXPECT_NEAR(0, agent_->PV(0,i,0,A), finance_eps);  // discount time = 0
  EXPECT_NEAR(A / (1+i), agent_->PV(1,i,0,A), finance_eps); // discount time = 1
  EXPECT_NEAR(A * (1 - std::pow((1 + i), -n)) / i, agent_->PV(n,i,0,A), finance_eps); // general
  EXPECT_NEAR(A * (1 - std::pow((1 + (i/12)), -n*12)) / (i/12), agent_->PV(n*12,i/12,0,A), finance_eps); // general
  EXPECT_NEAR(A * (1 - std::pow((1 + i), n)) / i, agent_->PV(-n,i,0,A), finance_eps); // negative time

  EXPECT_NEAR(F + A, agent_->PV(n,0,F,A), finance_eps); // discount rate = 0
  EXPECT_NEAR(F, agent_->PV(0,i,F,A), finance_eps); // discount time = 0
  EXPECT_NEAR((F + A) / (1+i), agent_->PV(1,i,F,A), finance_eps); // discount time = 1
  EXPECT_NEAR(F / std::pow((1+i), n) + A * (1 - std::pow((1 + i), -n)) / i,
              agent_->PV(n,i,F,A), finance_eps); // general

}


TEST_P(AgentTests, FV) {

  int n = 5;
  double i = 0.1;
  double P = 1;
  double A = 0.5;
  double finance_eps = 1e-4;

  // testing by "round trip" with already tested functions
  EXPECT_NEAR(P, agent_->PV(n,0,agent_->FV(n,0,P,0),0), finance_eps);  // discount rate = 0
  EXPECT_NEAR(P, agent_->PV(0,i,agent_->FV(0,i,P,0),0), finance_eps);  // discount time = 0
  EXPECT_NEAR(P, agent_->PV(i,i,agent_->FV(1,i,P,0),0), finance_eps);  // discount time = 1
  EXPECT_NEAR(P, agent_->PV(n,i,agent_->FV(n,i,P,0),0), finance_eps);  // general
  EXPECT_NEAR(P, agent_->PV(n*12,i/12,agent_->FV(n*12,i/12,P,0),0), finance_eps);  // general
  EXPECT_NEAR(P, agent_->PV(-n,i,agent_->FV(-n,i,P,0),0), finance_eps);  // general
  
  // manual testing
  EXPECT_NEAR(A, agent_->FV(n,0,0,A), finance_eps); // discount rate = 0
  EXPECT_NEAR(0, agent_->FV(0,i,0,A), finance_eps);  // discount time = 0
  EXPECT_NEAR(A, agent_->FV(1,i,0,A), finance_eps); // discount time = 1
  EXPECT_NEAR(A * (std::pow((1 + i), n) - 1) / i, agent_->PV(n,i,0,A), finance_eps); // general
  EXPECT_NEAR(A * (std::pow((1 + i/12), n*12) - 1) / i/12, agent_->PV(n*12,i/12,0,A), finance_eps); // general
  EXPECT_NEAR(A * (std::pow((1 + i), -n) - 1) / i, agent_->PV(-n,i,0,A), finance_eps); // general

  EXPECT_NEAR(P + A, agent_->FV(n,0,P,A), finance_eps); // discount rate = 0
  EXPECT_NEAR(P, agent_->FV(0,i,P,A), finance_eps); // discount time = 0
  EXPECT_NEAR(P * (1+i) + A, agent_->FV(1,i,P,A), finance_eps); // discount time = 1
  EXPECT_NEAR(P * std::pow((1+i), n) + A * (std::pow((1 + i), n) - 1),
              agent_->FV(n,i,P,A), finance_eps); // general

}

TEST_P(AgentTests, PMT) {

  int n = 5;
  double i = 0.1;
  double P = 1.5;
  double F = 3.5;
  double finance_eps = 1e-4;

  // all testing by "round trip" with already tested functions

  EXPECT_NEAR(P, agent_->PV(n,0,0,agent_->PMT(n,0,P,0)), finance_eps); // discount rate = 0
  EXPECT_NEAR(P, agent_->PV(0,i,0,agent_->PMT(0,i,P,0)), finance_eps); // discount time = 0
  EXPECT_NEAR(P, agent_->PV(1,i,0,agent_->PMT(1,i,P,0)), finance_eps); // discount time = 1
  EXPECT_NEAR(P, agent_->PV(n,i,0,agent_->PMT(n,i,P,0)), finance_eps); // general
  
  EXPECT_NEAR(F, agent_->FV(n,0,0,agent_->PMT(n,0,0,F)), finance_eps); // discount rate = 0
  EXPECT_NEAR(F, agent_->FV(0,i,0,agent_->PMT(0,i,0,F)), finance_eps); // discount time = 0
  EXPECT_NEAR(F, agent_->FV(1,i,0,agent_->PMT(1,i,0,F)), finance_eps); // discount time = 1
  EXPECT_NEAR(F, agent_->FV(n,i,0,agent_->PMT(n,i,0,F)), finance_eps); // general

  EXPECT_NEAR(P + agent_->PV(n,i,F,0), agent_->PV(n,i,0,agent_->PMT(n,i,P,F)), finance_eps);

}


TEST_P(AgentTests, PVA) {

  std::vector<double> A1({1.0,1.0,1.0,1.0,1.0});
  std::vector<double> A2({1.0,2.0,3.0,3.5,4.71});

  int n = A1.size();
  double i = 0.1;
  double finance_eps = 1e-4;

  EXPECT_NEAR(agent_->PV(n,i,0,A1[0]), agent_->PV(i, A1), finance_eps);
  double PV_A2 = 0;
  for (int n2=0; n2 < A2.size(); n2++) {
    PV_A2 += agent_->PV(n2+1, i, 0, A2[n2]);
  }
  EXPECT_NEAR(PV_A2, agent_->PV(i, A2), finance_eps);

}

