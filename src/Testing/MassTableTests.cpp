// MassTableTests.cpp
#include <gtest/gtest.h>
#include "MassTableTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MassTableTest, gramsPerMol){
  EXPECT_NEAR(235, cyclus::MT->gramsPerMol(u235_),0.5);
  EXPECT_NEAR(241, cyclus::MT->gramsPerMol(am241_),0.5);
  EXPECT_NEAR(228, cyclus::MT->gramsPerMol(th228_),0.5);
  EXPECT_NEAR(208, cyclus::MT->gramsPerMol(pb208_), 0.5);
}

