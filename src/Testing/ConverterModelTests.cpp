// ConverterModelTests.cpp 
#include <gtest/gtest.h>

#include "ConverterModelTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ConverterModelTests, ConverterType) {
  EXPECT_EQ(converter_model_->getModelType(),"Converter");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ConverterModelTests, SetAndGetConvName) {
  EXPECT_NO_THROW(converter_model_->setConvName("test_name"));
  EXPECT_EQ(converter_model_->getConvName(),"test_name");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ConverterModelTests, CopyFreshModel) {
  ConverterModel* new_stub = converter_model_;
  EXPECT_NO_THROW(new_stub->copyFreshModel(dynamic_cast<Model*>(converter_model_))); 
}

