// model_tests.cc 
#include <sstream>
#include <string>
#include <gtest/gtest.h>

#include "model_tests.h"
#include "xml_parser.h"
#include "xml_query_engine.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ModelTests, DISABLED_CreateFromXML) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ModelTests, DISABLED_LoadConstructor) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ModelTests, DISABLED_InitFromXML) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ModelTests, Print) {
  std::string s = model_->str();
  EXPECT_NO_THROW(std::string s = model_->str());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ModelTests, Schema) {
  std::stringstream schema;
  schema << ("<element name=\"foo\">\n");
  schema << model_->schema();
  schema << "</element>\n";
  cyclus::XMLParser p;
  EXPECT_NO_THROW(p.Init(schema));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ModelTests, DISABLED_InitAndGetName) {
  cyclus::XMLParser p;
  std::stringstream ss;
  ss << "<start><name>fooname</name></start>";
  p.Init(ss);
  cyclus::XMLQueryEngine engine(p);
  model_->Model::InitFrom(&engine);
  EXPECT_EQ(model_->prototype(),"fooname");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ModelTests, GetModelType) {
  EXPECT_NE(std::string("Model"), model_->kind());
}

