// CompMapTests.cpp 
#include <gtest/gtest.h>
#include <iostream>
#include "CompMapTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,default_constructor) {
  EXPECT_EQ(comp_.basis(),basis_);
  EXPECT_EQ(map_,comp_.map());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,copy_constructor) {
  CompMap copy = CompMap(comp_);
  EXPECT_EQ(copy.basis(),comp_.basis());
  EXPECT_EQ(copy.map(),comp_.map());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,map_interface) {
  LoadMap();
  for (Map::iterator it = map_.begin(); it != map_.end(); it++) {
    EXPECT_NO_THROW(comp_[it->first] = it->second);
  }
  EXPECT_EQ(*map_.begin(),*comp_.begin());
  EXPECT_NO_THROW(comp_.end());
  for (CompMap::iterator it = comp_.begin(); it != comp_.end(); it++) {
    EXPECT_EQ(map_.count(it->first),comp_.count(it->first));
    EXPECT_EQ(map_[it->first],comp_[it->first]);
    comp_.erase(it->first);
  }
  EXPECT_TRUE(comp_.empty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,normalize) { 
  LoadMap();
  comp_.setMap(map_);
  EXPECT_EQ(map_,comp_.map());
  EXPECT_NO_THROW(comp_.normalize());
  EXPECT_EQ(massified_,comp_.map());
  EXPECT_DOUBLE_EQ(ratio_,comp_.mass_to_atom_ratio());
  EXPECT_TRUE(comp_.normalized());
  for (CompMap::iterator it = comp_.begin(); it != comp_.end(); it++) {
    EXPECT_DOUBLE_EQ(massified_[it->first],comp_.massFraction(it->first));
    EXPECT_DOUBLE_EQ(atomified_[it->first],comp_.atomFraction(it->first));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,atomify) { 
  LoadMap();
  comp_.setMap(map_);
  EXPECT_NO_THROW(comp_.atomify());
  EXPECT_DOUBLE_EQ(ratio_,comp_.mass_to_atom_ratio());
  EXPECT_TRUE(comp_.normalized());
  for (CompMap::iterator it = comp_.begin(); it != comp_.end(); it++) {
    EXPECT_DOUBLE_EQ(massified_[it->first],comp_.massFraction(it->first));
    EXPECT_DOUBLE_EQ(atomified_[it->first],comp_.atomFraction(it->first));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,massify) { 
  LoadMap();
  comp_.setMap(map_);
  EXPECT_NO_THROW(comp_.atomify());  
  EXPECT_NO_THROW(comp_.massify());  
  EXPECT_DOUBLE_EQ(ratio_,comp_.mass_to_atom_ratio());
  EXPECT_TRUE(comp_.normalized());
  for (CompMap::iterator it = comp_.begin(); it != comp_.end(); it++) {
    EXPECT_DOUBLE_EQ(massified_[it->first],comp_.massFraction(it->first));
    EXPECT_DOUBLE_EQ(atomified_[it->first],comp_.atomFraction(it->first));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,lineage) { 
  LoadLineage();
  EXPECT_EQ(parent, child->parent());
  EXPECT_EQ(root, child->root_comp());
  EXPECT_EQ(root_decay_time, child->root_decay_time());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,empty_comp_behaviors) {
  comp_[92235]=0;
  comp_.normalize();
  EXPECT_FLOAT_EQ(0,comp_.atomFraction(92235));
  EXPECT_FLOAT_EQ(0,comp_.massFraction(92235));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,equality) {
  LoadMap();
  comp_.setMap(map_);
  comp_.normalize();
  CompMap copy = CompMap(comp_);
  EXPECT_TRUE(copy == comp_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests, almostEquality) {
  LoadMap();
  comp_.setMap(map_);
  comp_.normalize();
  CompMap copy = CompMap(comp_);
  CompMap::iterator it;
  for(it=copy.begin(); it!=copy.end(); ++it){
    (*it).second += 1.0;
  }
  EXPECT_FALSE(copy == comp_);
  EXPECT_TRUE(copy.almostEqual(comp_, 2.0));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests, almostEqualNegThresh) {
  LoadMap();
  comp_.setMap(map_);
  comp_.normalize();
  CompMap copy = CompMap(comp_);
  EXPECT_THROW(copy.almostEqual(comp_, -1.0), CycNegativeValueException);
}


