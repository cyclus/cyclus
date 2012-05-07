// CompMapTests.cpp 
#include <gtest/gtest.h>

#include "CompMapTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,default_constructor) {
  EXPECT_EQ(comp.basis(),basis);
  EXPECT_EQ(map,comp.map());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,copy_constructor) {
  CompMap copy = CompMap(comp);
  EXPECT_EQ(copy.basis(),comp.basis());
  EXPECT_EQ(copy.map(),comp.map());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,map_interface) {
  LoadMap();
  for (Map::iterator it = map.begin(); it != map.end(); it++) {
    EXPECT_NO_THROW(comp[it->first] = it->second);
  }
  EXPECT_EQ(*map.begin(),*comp.begin());
  EXPECT_NO_THROW(comp.end());
  for (CompMap::iterator it = comp.begin(); it != comp.end(); it++) {
    EXPECT_EQ(map.count(it->first),comp.count(it->first));
    EXPECT_EQ(map[it->first],comp[it->first]);
    comp.erase(it->first);
  }
  EXPECT_TRUE(comp.empty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,normalize) { 
  LoadMap();
  comp.setMap(map);
  EXPECT_EQ(map,comp.map());
  EXPECT_NO_THROW(comp.normalize());
  EXPECT_EQ(massified,comp.map());
  EXPECT_DOUBLE_EQ(ratio,comp.mass_to_atom_ratio());
  EXPECT_TRUE(comp.normalized());
  for (CompMap::iterator it = comp.begin(); it != comp.end(); it++) {
    EXPECT_DOUBLE_EQ(massified[it->first],comp.massFraction(it->first));
    EXPECT_DOUBLE_EQ(atomified[it->first],comp.atomFraction(it->first));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,atomize) { 
  LoadMap();
  comp.setMap(map);
  EXPECT_NO_THROW(comp.atomify());
  EXPECT_DOUBLE_EQ(ratio,comp.mass_to_atom_ratio());
  EXPECT_TRUE(comp.normalized());
  for (CompMap::iterator it = comp.begin(); it != comp.end(); it++) {
    EXPECT_DOUBLE_EQ(massified[it->first],comp.massFraction(it->first));
    EXPECT_DOUBLE_EQ(atomified[it->first],comp.atomFraction(it->first));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,massify) { 
  LoadMap();
  comp.setMap(map);
  EXPECT_NO_THROW(comp.atomify());  
  EXPECT_NO_THROW(comp.massify());  
  EXPECT_DOUBLE_EQ(ratio,comp.mass_to_atom_ratio());
  EXPECT_TRUE(comp.normalized());
  for (CompMap::iterator it = comp.begin(); it != comp.end(); it++) {
    EXPECT_DOUBLE_EQ(massified[it->first],comp.massFraction(it->first));
    EXPECT_DOUBLE_EQ(atomified[it->first],comp.atomFraction(it->first));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,lineage) { 
  LoadLineage();
  EXPECT_EQ(parent,child->parent());
  EXPECT_EQ(root,child->root_comp());
  EXPECT_EQ(root_decay_time,child->root_decay_time());
}
