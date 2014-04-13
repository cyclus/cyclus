// agent_tests.cc
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "agent_tests.h"
#include "infile_tree.h"
#include "xml_parser.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_P(AgentTests, DISABLED_CreateFromXML) {
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_P(AgentTests, DISABLED_LoadConstructor) {
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_P(AgentTests, DISABLED_InitFromXML) {
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_P(AgentTests, Print) {
  std::string s = agent_->str();
  EXPECT_NO_THROW(std::string s = agent_->str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_P(AgentTests, Schema) {
  std::stringstream schema;
  schema << ("<element name=\"foo\">\n");
  schema << agent_->schema();
  schema << "</element>\n";
  cyclus::XMLParser p;
  EXPECT_NO_THROW(p.Init(schema));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_P(AgentTests, GetAgentType) {
  EXPECT_NE(std::string("Agent"), agent_->kind());
}
