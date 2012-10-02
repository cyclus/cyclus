// ConverterModelTests.cpp 
#include <gtest/gtest.h>

#include "ConverterModelTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ConverterModelTests, ConverterType) {
  EXPECT_EQ(converter_model_->modelType(),"Converter");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ConverterModelTests, SetAndGetConvName) {
  EXPECT_NO_THROW(converter_model_->setConvName("test_name"));
  EXPECT_EQ(converter_model_->getConvName(),"test_name");
}

