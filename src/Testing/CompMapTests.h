// CompMapTests.h
#include <gtest/gtest.h>

#include "CompMap.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class TestCompMap : public CompMap {
 public:  
 TestCompMap() : CompMap(MASS) {};
 TestCompMap(Basis b) : CompMap(b) {};
  
  Map map_;
  void setMap(Map m) {map_ = m;}
  
  CompMapPtr parent_;  
  void setParent(CompMapPtr p) {parent_ = p;}
  
  double decay_time_;
  void setDecayTime(double t) {decay_time_ = t;}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CompMapTests : public ::testing::Test {
 protected:
  TestCompMap comp;
  Map map;
  Basis basis;
  CompMapPtr child, parent, root;
  double t1, t2, total;
  int iso_base, nSpecies;

 public:
  virtual void SetUp() { 
    basis = MASS;
    map = Map();
    comp = TestCompMap(basis);
  }

  virtual void TearDown() {}

  void LoadMap() {
    iso_base = 1000;
    nSpecies = 2;
    for (int i = 0; i < nSpecies; i++) {
      map[iso_base+i+1] = (iso_base+i) * 1.0;
    }
  }
};

