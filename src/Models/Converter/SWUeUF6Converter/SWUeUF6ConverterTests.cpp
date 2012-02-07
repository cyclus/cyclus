// SWUeUF6ConverterTests.cpp
#include <gtest/gtest.h>

#include "SWUeUF6Converter.h"
#include "CycException.h"
#include "Message.h"
#include "ConverterModelTests.h"
#include "ModelTests.h"
#include "TestInst.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeSWUeUF6Converter : public SWUeUF6Converter {
  public:
    FakeSWUeUF6Converter() : SWUeUF6Converter() {
    }

    virtual ~FakeSWUeUF6Converter() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* SWUeUF6ConverterModelConstructor(){
  return dynamic_cast<Model*>(new FakeSWUeUF6Converter());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ConverterModel* SWUeUF6ConverterConstructor(){
  return dynamic_cast<ConverterModel*>(new FakeSWUeUF6Converter());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SWUeUF6ConverterTest : public ::testing::Test {
  protected:
    FakeSWUeUF6Converter* src_facility;
    FakeSWUeUF6Converter* new_facility; 

    virtual void SetUp(){
      src_facility = new FakeSWUeUF6Converter();
      src_facility->setParent(new TestInst());
      new_facility = new FakeSWUeUF6Converter();
      // for facilities that trade commodities, create appropriate markets here
    };

    virtual void TearDown() {
      delete src_facility;
      delete new_facility;
      // for facilities that trade commodities, delete appropriate markets here
    }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SWUeUF6ConverterTest, InitialState) {
  // Test things about the initial state of the facility here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SWUeUF6ConverterTest, CopyFreshModel) {
  new_facility->copyFreshModel(dynamic_cast<Model*>(src_facility)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<SWUeUF6Converter*>(new_facility)); // still a stub facility
  EXPECT_NO_THROW(dynamic_cast<FakeSWUeUF6Converter*>(new_facility)); // still a fake stub facility
  // Test that SWUeUF6Converter specific parameters are initialized in the deep copy method here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SWUeUF6ConverterTest, Print) {
  EXPECT_NO_THROW(src_facility->print());
  // Test SWUeUF6Converter specific aspects of the print method here
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SWUeUF6ConverterTest, ReceiveMessage) {
  msg_ptr msg;
  // Test SWUeUF6Converter specific behaviors of the receiveMessage function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(SWUeUF6Conv, ConverterModelTests, Values(&SWUeUF6ConverterConstructor));
INSTANTIATE_TEST_CASE_P(SWUeUF6Conv, ModelTests, Values(&SWUeUF6ConverterModelConstructor));

