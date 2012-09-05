// IsoVectorTests.cpp 
#include <gtest/gtest.h>

#include "IsoVectorTests.h"

#include "CycException.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,default_constructor) {
  CompMapPtr zero_comp = CompMapPtr(new CompMap(MASS));
  EXPECT_TRUE(zero_vec.compEquals(zero_comp));
}

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
  EXPECT_NO_THROW(orig_vec += vec_to_add);
  EXPECT_TRUE(orig_vec.compEquals(*add_result));
  for (CompMap::iterator it = orig_vec.comp()->begin(); it != orig_vec.comp()->end(); it++) {
    EXPECT_DOUBLE_EQ((*add_result)[it->first],it->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,mixing) {
  LoadMaps();
  EXPECT_NO_THROW(orig_vec.mix(vec_to_add,ratio));
  EXPECT_TRUE(orig_vec.compEquals(*mix_result));
  EXPECT_THROW(orig_vec.mix(vec_to_add,-1),CycRangeException);
  EXPECT_NO_THROW(orig_vec.mix(vec_to_add,0)); // corner
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,mixing_zero_vector) { 
  LoadMaps();
  EXPECT_NO_THROW(zero_vec.mix(vec_to_add, .1)); // should just ignore ratio
  EXPECT_TRUE(zero_vec.compEquals(vec_to_add)); // the resulting vector is the added vector
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,intersection_fraction) {
  LoadMaps();
  orig_vec += vec_to_add;
  double amt;
  EXPECT_NO_THROW(amt = orig_vec.intersectionFraction(vec_to_add));
  EXPECT_DOUBLE_EQ(amt,0.5);
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
  /// \@MJG_FLAG this needs to be written... think about the best way to do it
}
