// MassTableTests.cpp
#include <gtest/gtest.h>
#include "MassTableTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MassTableTest, gramsPerMol){
  EXPECT_NEAR(235, MT->gramsPerMol(u235_),0.5);
  EXPECT_NEAR(241, MT->gramsPerMol(am241_),0.5);
  EXPECT_NEAR(228, MT->gramsPerMol(th228_),0.5);
  EXPECT_NEAR(208, MT->gramsPerMol(pb208_), 0.5);
}

