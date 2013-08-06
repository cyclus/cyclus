// iso_vector_tests.cc 
#include <gtest/gtest.h>

#include "iso_vector_tests.h"

#include "error.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,default_constructor) {
  using cyclus::IsoVector;
  using cyclus::CompMap;
  using cyclus::CompMapPtr;
  using cyclus::MASS;
  CompMapPtr zero_comp = CompMapPtr(new CompMap(MASS));
  EXPECT_TRUE(zero_vec.CompEquals(zero_comp));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,constructors) {
  using cyclus::IsoVector;
  using cyclus::CompMap;
  using cyclus::CompMapPtr;
  using cyclus::MASS;
  IsoVector equality = IsoVector(comp);
  EXPECT_EQ(equality,vec); // equality
  IsoVector copy = IsoVector(vec);
  EXPECT_EQ(copy,vec); // copy
  IsoVector assignment = vec;
  EXPECT_EQ(assignment,vec); // assignment
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,equivalence) {
  using cyclus::IsoVector;
  using cyclus::CompMap;
  using cyclus::CompMapPtr;
  using cyclus::MASS;
  LoadMaps();
  IsoVector v1 = IsoVector(subtract_result);
  IsoVector v2 = IsoVector(subtract_result);
  EXPECT_EQ(v1,v2);
  EXPECT_TRUE(v1 == v2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,addition) {
  using cyclus::IsoVector;
  using cyclus::CompMap;
  using cyclus::CompMapPtr;
  using cyclus::MASS;
  LoadMaps();
  EXPECT_NO_THROW(add_to_vec += to_add_vec);
  EXPECT_TRUE(add_to_vec.CompEquals(add_result));
  for (CompMap::iterator it = add_to_vec.comp()->begin(); it != add_to_vec.comp()->end(); it++) {
    EXPECT_DOUBLE_EQ((*add_result)[it->first],it->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,simple_mixing) {
  using cyclus::IsoVector;
  using cyclus::CompMap;
  using cyclus::CompMapPtr;
  using cyclus::MASS;
  LoadMaps();
  IsoVector v1 = IsoVector(subtract_result);
  IsoVector v2 = IsoVector(subtract_result);
  EXPECT_EQ(v1,v2);
  v1.Mix(v2,1);
  EXPECT_TRUE(v1.CompEquals(subtract_result));
  EXPECT_EQ(v1,v2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,mixing) {
  using cyclus::IsoVector;
  using cyclus::CompMap;
  using cyclus::CompMapPtr;
  using cyclus::MASS;
  LoadMaps();
  EXPECT_NO_THROW(add_to_vec.Mix(to_add_vec,ratio));
  EXPECT_TRUE(add_to_vec.CompEquals(mix_result));
  EXPECT_THROW(add_to_vec.Mix(to_add_vec,-1), cyclus::ValueError);
  EXPECT_NO_THROW(add_to_vec.Mix(to_add_vec,0)); // corner
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,mixing_zero_vector) { 
  using cyclus::IsoVector;
  using cyclus::CompMap;
  using cyclus::CompMapPtr;
  using cyclus::MASS;
  LoadMaps();
  EXPECT_NO_THROW(zero_vec.Mix(to_add_vec, .1)); // should just ignore ratio
  EXPECT_TRUE(zero_vec.CompEquals(to_add_vec)); // the resulting vector is the added vector
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,intersection_fraction) {
  LoadMaps();
  add_to_vec += to_add_vec;
  double amt;
  EXPECT_NO_THROW(amt = add_to_vec.IntersectionFraction(to_add_vec));
  EXPECT_DOUBLE_EQ(amt,0.5);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,subtraction) {
  using cyclus::IsoVector;
  using cyclus::CompMap;
  using cyclus::CompMapPtr;
  using cyclus::MASS;
  LoadMaps();
  EXPECT_NO_THROW(subtract_from_vec -= to_subtract_vec);
  EXPECT_TRUE(subtract_from_vec.CompEquals(subtract_result));
  for (CompMap::iterator it = subtract_from_vec.comp()->begin(); it != subtract_from_vec.comp()->end(); it++) {
    EXPECT_DOUBLE_EQ((*subtract_result)[it->first],it->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,separation) {
  LoadMaps();
  EXPECT_NO_THROW(subtract_from_vec.Separate(to_subtract_vec,efficiency));
  EXPECT_TRUE(subtract_from_vec.CompEquals(separate_result));
  EXPECT_THROW(subtract_from_vec.Separate(to_subtract_vec,-1),cyclus::ValueError);
  EXPECT_THROW(subtract_from_vec.Separate(to_subtract_vec,2),cyclus::ValueError);
  EXPECT_NO_THROW(subtract_from_vec.Separate(to_subtract_vec,0)); // corner
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,decay) {
  /// \@MJG_FLAG this needs to be written... think about the best way to do it
}
