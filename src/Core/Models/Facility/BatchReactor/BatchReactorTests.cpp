// BatchReactorTests.cpp
#include <gtest/gtest.h>

#include "BatchReactorTests.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(BatchReactorTest, InitialState) {
  // Test things about the initial state of the facility here
  EXPECT_EQ(lencycle,src_facility->cycleLength());
  EXPECT_EQ(life,src_facility->lifetime());
  EXPECT_EQ(loadcore,src_facility->coreLoading());
  EXPECT_EQ(nbatch,src_facility->nBatches());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(BatchReactorTest, CopyFreshModel) {
  new_facility->copyFreshModel(dynamic_cast<Model*>(src_facility)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<BatchReactor*>(new_facility)); // still a recipe reactor
  EXPECT_NO_THROW(dynamic_cast<FakeBatchReactor*>(new_facility)); // still a fake recipe reactor
  // Test that BatchReactor specific parameters are initialized in the deep copy method here
  EXPECT_EQ( lencycle, new_facility->cycleLength() );
  EXPECT_EQ( life, new_facility->lifetime() );
  EXPECT_EQ( loadcore, new_facility->coreLoading() );
  EXPECT_EQ( nbatch, new_facility->nBatches() );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(BatchReactorTest, Print) {
  EXPECT_NO_THROW(std::string s = src_facility->str());
  //Test BatchReactor specific aspects of the print method here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(BatchReactorTest, ReceiveMessage) {
  msg_ptr msg;
  //Test BatchReactor specific behaviors of the receiveMessage function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(BatchReactorTest, Tick) {
  int time = 1;
  EXPECT_NO_THROW(src_facility->handleTick(time));
  //Test BatchReactor specific behaviors of the handleTick function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(BatchReactorTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_facility->handleTick(time));
  //Test BatchReactor specific behaviors of the handleTock function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(BatchReactor, FacilityModelTests, Values(&BatchReactorConstructor));
INSTANTIATE_TEST_CASE_P(BatchReactor, ModelTests, Values(&BatchReactorModelConstructor));

