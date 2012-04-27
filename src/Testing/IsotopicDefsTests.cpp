#include <gtest/gtest.h>

#include "IsotopicDefinitions.h"

#include <utility>

class IsotopicDefsTests : public ::testing::Test {
protected:
  CompMap_p pfracs;
  int iso;
  double value;

  comp_t comp;
  comp_p pcomp;

  // this sets up the fixtures
  virtual void SetUp() {
    pfracs.reset(new CompMap());
    iso = 1001;
    value = 2;
    (*pfracs)[iso] = value;
    pcomp = comp_p(new comp_t);
    comp = *pcomp;
  };
  
  // this tears down the fixtures
  virtual void TearDown() {
  }

  void init() {
    pcomp = comp_p(new comp_t(pfracs));
    comp = *pcomp;
  }
  double comp_sum() {
    double total = 0;
    for (CompMap::iterator it = comp.mass_fractions->begin(); 
         it != comp.mass_fractions->end(); it++) {
      total += it->second;
    }
    return total;
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsotopicDefsTests, empty_constructor) {
  comp_t test_comp = comp_t(); // empty constructor
  EXPECT_EQ(test_comp.ID,comp.ID);
  EXPECT_EQ(test_comp.decay_time,comp.decay_time);
  EXPECT_EQ(comp.nSpecies(),0);
  EXPECT_EQ(test_comp.nSpecies(),0);
  EXPECT_EQ(test_comp.parent(),comp.parent());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsotopicDefsTests, logged) {
  EXPECT_EQ( comp.logged(), false ); // id 0, nspecies 0
  comp.ID++;
  EXPECT_EQ( comp.logged(), false ); // id > 0, nspecies 0
  comp.ID--;
  init();
  EXPECT_EQ( comp.logged(), false ); // id 0, nspecies > 0
  comp.ID++;
  EXPECT_EQ( comp.logged(), true ); // id > 0, ncpecies > 0
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsotopicDefsTests, parent) { 
  comp_p ptest_rent = comp_p(new comp_t);
  comp.setParent(ptest_rent);
  EXPECT_EQ( ptest_rent, comp.parent() );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsotopicDefsTests, other_constructors) {
  init();
  comp_t test_comp = comp_t(pfracs); // CompMap_p constructor 
  EXPECT_EQ( test_comp, comp );
  comp_t test_comp2 = comp_t(comp); // copy constructor
  EXPECT_EQ( test_comp2, comp );
  comp_t test_comp3;
  EXPECT_NE( test_comp3, comp );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsotopicDefsTests, pointers) {
  init();
  comp_p test_pcomp = comp_p(pcomp);
  EXPECT_EQ( test_pcomp, pcomp );
  EXPECT_EQ( *test_pcomp, *pcomp );
  EXPECT_EQ( pcomp->me(), pcomp );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsotopicDefsTests, normalize) {
  init();
  double total1 = comp_sum();
  comp.normalize();
  double total2 = comp_sum();
  EXPECT_NE( total1, total2 );
  EXPECT_EQ( total2, 1 );
}
