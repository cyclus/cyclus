// comp_map_tests.cc 
#include <gtest/gtest.h>
#include <iostream>
#include "comp_map_tests.h"
#include "error.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,default_constructor) {
  EXPECT_EQ(comp_->basis(),basis_);
  EXPECT_EQ(map_,comp_->map());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,copy_constructor) {
  using cyclus::CompMap;
  CompMap copy = CompMap(*comp_);
  EXPECT_EQ(copy.basis(),comp_->basis());
  EXPECT_EQ(copy.map(),comp_->map());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,map_interface) {
  using cyclus::Map;
  using cyclus::CompMap;
  LoadMap();
  for (Map::iterator it = map_.begin(); it != map_.end(); it++) {
    EXPECT_NO_THROW((*comp_)[it->first] = it->second);
  }
  EXPECT_EQ(*map_.begin(),*comp_->begin());
  EXPECT_NO_THROW(comp_->end());
  for (CompMap::Iterator it = comp_->begin(); it != comp_->end(); it++) {
    EXPECT_EQ(map_.count(it->first),comp_->count(it->first));
    EXPECT_EQ(map_[it->first],(*comp_)[it->first]);
    comp_->erase(it->first);
  }
  EXPECT_TRUE(comp_->empty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,normalize) { 
  using cyclus::CompMap;
  LoadMap();
  comp_->SetMap(map_);
  EXPECT_EQ(map_,comp_->map());
  EXPECT_NO_THROW(comp_->normalize());
  EXPECT_EQ(massified_,comp_->map());
  EXPECT_DOUBLE_EQ(ratio_,comp_->mass_to_atom_ratio());
  EXPECT_TRUE(comp_->normalized());
  for (CompMap::Iterator it = comp_->begin(); it != comp_->end(); it++) {
    EXPECT_DOUBLE_EQ(massified_[it->first],comp_->MassFraction(it->first));
    EXPECT_DOUBLE_EQ(atomified_[it->first],comp_->AtomFraction(it->first));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,atomify) { 
  using cyclus::CompMap;
  LoadMap();
  comp_->SetMap(map_);
  EXPECT_NO_THROW(comp_->Atomify());
  EXPECT_DOUBLE_EQ(ratio_,comp_->mass_to_atom_ratio());
  EXPECT_TRUE(comp_->normalized());
  for (CompMap::Iterator it = comp_->begin(); it != comp_->end(); it++) {
    EXPECT_DOUBLE_EQ(massified_[it->first],comp_->MassFraction(it->first));
    EXPECT_DOUBLE_EQ(atomified_[it->first],comp_->AtomFraction(it->first));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,massify) { 
  using cyclus::CompMap;
  LoadMap();
  comp_->SetMap(map_);
  EXPECT_NO_THROW(comp_->Atomify());  
  EXPECT_NO_THROW(comp_->Massify());  
  EXPECT_DOUBLE_EQ(ratio_,comp_->mass_to_atom_ratio());
  EXPECT_TRUE(comp_->normalized());
  for (CompMap::Iterator it = comp_->begin(); it != comp_->end(); it++) {
    EXPECT_DOUBLE_EQ(massified_[it->first],comp_->MassFraction(it->first));
    EXPECT_DOUBLE_EQ(atomified_[it->first],comp_->AtomFraction(it->first));
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
  (*comp_)[92235]=0;
  comp_->normalize();
  EXPECT_FLOAT_EQ(0,comp_->AtomFraction(92235));
  EXPECT_FLOAT_EQ(0,comp_->MassFraction(92235));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,equality) {
  using cyclus::CompMapPtr;
  LoadMap();
  comp_->SetMap(map_);
  comp_->normalize();
  CompMapPtr copy = CompMapPtr(comp_);
  EXPECT_TRUE(copy == comp_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests, almostEquality) {
  using cyclus::CompMap;
  LoadMap();
  comp_->SetMap(map_);
  comp_->normalize();
  CompMap copy = CompMap(*comp_);
  CompMap::Iterator it;
  for(it=copy.begin(); it!=copy.end(); ++it){
    (*it).second *= 1.1;
  }
  EXPECT_FALSE(copy == *comp_);
  EXPECT_TRUE(copy.AlmostEqual(*comp_, 1.1));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests, almostEqualZeroEntry) {
  using cyclus::CompMap;
  LoadMap();
  comp_->SetMap(map_);
  comp_->normalize();
  CompMap copy = CompMap(*comp_);
  CompMap::Iterator it;
  double the_max = 0.0;
  for(it=copy.begin(); it!=copy.end(); ++it){
    (*it).second = 0;
  }
  EXPECT_FALSE(copy == *comp_);
  EXPECT_TRUE(copy.AlmostEqual(*comp_, 1.1));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests, almostEqualNegThresh) {
  using cyclus::CompMap;
  LoadMap();
  comp_->SetMap(map_);
  comp_->normalize();
  CompMap copy = CompMap(*comp_);
  EXPECT_THROW(copy.AlmostEqual(*comp_, -1.0), cyclus::ValueError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests, zerocomp){
  int u235=92235;
  cyclus::CompMapPtr comp = cyclus::CompMapPtr(new cyclus::CompMap(cyclus::MASS));
  (*comp)[u235] = 0;
  EXPECT_FLOAT_EQ(0, (*comp)[u235]);
  comp->Massify();
  EXPECT_FLOAT_EQ(0, (*comp)[u235]);
}

