// IsoVectorTests.cpp 
#include <gtest/gtest.h>

#include "IsoVectorTests.h"

#include "CycException.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,constructors) {
  IsoVector equality = IsoVector(comp);
  EXPECT_EQ(equality,vec); // equality
  IsoVector copy = IsoVector(vec);
  EXPECT_EQ(copy,vec); // copy
  IsoVector assignment = vec;
  EXPECT_EQ(assignment,vec); // assignment
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,addition) {
  LoadMaps();
  EXPECT_NO_THROW(add_to_vec += to_add_vec);
  EXPECT_TRUE(add_to_vec.compEquals(*add_result));
  for (CompMap::iterator it = add_to_vec.comp()->begin(); it != add_to_vec.comp()->end(); it++) {
    EXPECT_DOUBLE_EQ((*add_result)[it->first],it->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,mixing) {
  LoadMaps();
  EXPECT_NO_THROW(add_to_vec.mix(to_add_vec,ratio));
  EXPECT_TRUE(add_to_vec.compEquals(*mix_result));
  EXPECT_THROW(add_to_vec.mix(to_add_vec,-1),CycRangeException);
  EXPECT_NO_THROW(add_to_vec.mix(to_add_vec,0)); // corner
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,subtraction) {
  LoadMaps();
  EXPECT_NO_THROW(subtract_from_vec -= to_subtract_vec);
  EXPECT_TRUE(subtract_from_vec.compEquals(*subtract_result));
  for (CompMap::iterator it = subtract_from_vec.comp()->begin(); it != subtract_from_vec.comp()->end(); it++) {
    EXPECT_DOUBLE_EQ((*subtract_result)[it->first],it->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,separation) {
  LoadMaps();
  EXPECT_NO_THROW(subtract_from_vec.separate(to_subtract_vec,efficiency));
  EXPECT_TRUE(subtract_from_vec.compEquals(*separate_result));
  EXPECT_THROW(subtract_from_vec.separate(to_subtract_vec,-1),CycRangeException);
  EXPECT_THROW(subtract_from_vec.separate(to_subtract_vec,2),CycRangeException);
  EXPECT_NO_THROW(subtract_from_vec.separate(to_subtract_vec,0)); // corner
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,decay) {
  /// @MJG_FLAG this needs to be written... think about the best way to do it
}
