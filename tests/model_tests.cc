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
TEST_P(ModelTests, InitAndGetName) {
  cyclus::XMLParser p;
  std::stringstream ss;
  ss << "<start><name>fooname</name></start>";
  p.Init(ss);
  cyclus::XMLQueryEngine engine(p);
  model_->Model::InitFrom(&engine);
  EXPECT_EQ(model_->name(),"fooname");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ModelTests, SetAndGetModelType) {
  for(int i=cyclus::REGION; i<cyclus::END_MODEL_TYPES; i++){
    switch(i){
      case (cyclus::REGION):
        EXPECT_NO_THROW(model_->SetModelType("Region"));
        EXPECT_EQ("Region", model_->ModelType());
        break;
      case (cyclus::INST):
        EXPECT_NO_THROW(model_->SetModelType("Inst"));
        EXPECT_EQ("Inst", model_->ModelType());
        break;
      case (cyclus::FACILITY):
        EXPECT_NO_THROW(model_->SetModelType("Facility"));
        EXPECT_EQ("Facility", model_->ModelType());
        break;
      default:
        FAIL();
        break;
    }
  }
}

