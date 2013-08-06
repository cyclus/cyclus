// Convertermodel_tests.cc 
#include <gtest/gtest.h>

#include "Convertermodel_tests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ConverterModelTests, ConverterType) {
  EXPECT_EQ(converter_model_->ModelType(),"Converter");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ConverterModelTests, SetAndGetConvName) {
  EXPECT_NO_THROW(converter_model_->SetConvName("test_name"));
  EXPECT_EQ(converter_model_->GetConvName(),"test_name");
}

