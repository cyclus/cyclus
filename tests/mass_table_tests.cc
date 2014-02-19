// mass_table_tests.cc
#include <gtest/gtest.h>
#include "mass_table_tests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MassTableTest, gramsPerMol){
  EXPECT_NEAR(235, cyclus::MT->GramsPerMol(u235_), 0.5);
  EXPECT_NEAR(241, cyclus::MT->GramsPerMol(am241_), 0.5);
  EXPECT_NEAR(228, cyclus::MT->GramsPerMol(th228_), 0.5);
  EXPECT_NEAR(208, cyclus::MT->GramsPerMol(pb208_), 0.5);
}

