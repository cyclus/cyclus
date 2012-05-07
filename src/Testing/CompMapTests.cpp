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
  EXPECT_EQ(comp.empty(),true);
}
