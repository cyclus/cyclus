// ModelTests.cpp 
#include <gtest/gtest.h>

#include "ModelTests.h"

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
  EXPECT_NO_THROW(model_->print());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ModelTests, SetAndGetName) {
  EXPECT_EQ("",model_->name());
  EXPECT_NO_THROW(model_->setName("test_name"));
  EXPECT_EQ(model_->name(),"test_name");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ModelTests, SetAndGetModelType) {
  for(int i=REGION; i<END_MODEL_TYPES; i++){
    switch(i){
      case (REGION):
        EXPECT_NO_THROW(model_->setModelType("Region"));
        EXPECT_EQ("Region", model_->getModelType());
        break;
      case (INST):
        EXPECT_NO_THROW(model_->setModelType("Inst"));
        EXPECT_EQ("Inst", model_->getModelType());
        break;
      case (FACILITY):
        EXPECT_NO_THROW(model_->setModelType("Facility"));
        EXPECT_EQ("Facility", model_->getModelType());
        break;
      case (MARKET):
        EXPECT_NO_THROW(model_->setModelType("Market"));
        EXPECT_EQ("Market", model_->getModelType());
        break;
      case (CONVERTER):
        EXPECT_NO_THROW(model_->setModelType("Converter"));
        EXPECT_EQ("Converter", model_->getModelType());
        break;
      default:
        FAIL();
        break;
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ModelTests, Copy) {
  Model* new_model = model_;
  EXPECT_NO_THROW(new_model->copy(model_)); 
}

