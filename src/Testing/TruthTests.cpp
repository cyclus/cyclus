#include <string>
#include <gtest/gtest.h>

TEST(TruthTest, Affirmative){
  // This test is named Affirtmative and belongs to the TruthTest 
  // test case
  // This is actually just testing that c++ knows how to define a boolean. 
  // It's a little redundant, since googletest relies on the same function 
  // in order to EXPECT_EQ
  bool first_truism = true;
  bool second_truism = true;
  EXPECT_EQ(first_truism, second_truism);
}


TEST(TruthTest, Negative){
  // This test is named Affirtmative and belongs to the TruthTest 
  // test case
  // This is actually just testing that c++ knows how to define a boolean. 
  // It's a little redundant, since googletest relies on the same function 
  // in order to EXPECT_FASLE
  bool truism = true;
  bool falsism = false;
  EXPECT_FALSE(truism = falsism);
}



