// MassTableTests.cpp
#include <gtest/gtest.h>
#include "MassTableTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MassTableTest, Constructors){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MassTableTest, gramsPerMol){
  EXPECT_FLOAT_EQ(235, MT->gramsPerMol(u235_));
  EXPECT_FLOAT_EQ(241, MT->gramsPerMol(am241_));
  EXPECT_FLOAT_EQ(228, MT->gramsPerMol(th228_));
  EXPECT_FLOAT_EQ(208, MT->gramsPerMol(pb208_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    


