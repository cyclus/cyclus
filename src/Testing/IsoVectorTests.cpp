#include <gtest/gtest.h>

#include "IsoVector.h"
#include "MassTable.h"

#include <vector>
#include "boost/assign.hpp"

using namespace std;

class IsoVectorTests : public ::testing::Test {
protected:
  IsoVectorPtr pcomp;

  int id;
  double decay_time, mass_to_atoms;
  bool parent_exists, logged;

  bool atom;
  CompMapPtr pfracs;
  int ntopes;
  double iso_base;
  vector<int> isotopes;
  vector<double> g_per_mol;
  vector<double> values;

  // this sets up the fixtures
  virtual void SetUp() {
    init();
    init_map_values();
    pcomp = IsoVectorPtr(new IsoVector(*pfracs,atom));
  };
  
  // this tears down the fixtures
  virtual void TearDown() {
    pfracs.reset();
    pcomp.reset();
  }

  void init() {
    isotopes = vector<int>();
    g_per_mol = vector<double>();
    values = vector<double>();
  }

  void init_map_values() {
    atom = true;
    int ntopes = 5;
    int iso_base = 1000;
    CompMap m;
    for (int i = 0; i < ntopes; i++) {
      int tope = iso_base+i+1;
      double value = 1.0*i+1;
      isotopes.push_back(tope);
      g_per_mol.push_back(MT->gramsPerMol(tope));
      values.push_back(value);
      m.insert(pair<int,double>(tope,value));
    }
    pfracs.reset(new CompMap(m));
  }

  void init_comp_values() {
    id = 0;
    decay_time = 0;
    parent_exists = false;
    logged = false;
    initialize(*pfracs,atom);
  }

  void initialize(CompMap& c, bool atom) {
    if (atom) {
      IsoVector::massify(c);
    }
    IsoVector::normalize(c);
  }

  void check_comp_map_eq(CompMap& c1, CompMap& c2) {
    int s1 = c1.size();
    int s2 = c2.size();
    EXPECT_EQ(s1,s2);
    for (CompMap::const_iterator it = c1.begin(); it != c1.end(); it++) {
      int key = it->first;
      int count = c2.count(key);
      EXPECT_EQ(count,1);
      EXPECT_EQ(c1[key],c2[key]);
    }
  }

  // double comp_sum() {
  //   double total = 0;
  //   for (CompMap::iterator it = comp.mass_fractions->begin(); 
  //        it != comp.mass_fractions->end(); it++) {
  //     total += it->second;
  //   }
  //   return total;
  // }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests, reference_transformations) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests, value_transformations) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests, constructor) {
  // set up  values
  init_comp_values();
  CompMap c1 = CompMap(*pcomp->comp());
  CompMap c2 = CompMap(*pfracs);
  // test values
  EXPECT_EQ(pcomp->ID(),id);
  EXPECT_EQ(pcomp->decay_time(),decay_time);
  EXPECT_EQ(pcomp->logged(),logged);
  check_comp_map_eq(c1,c2);
}

// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// TEST_F(IsotopicDefsTests, logged) {
//   EXPECT_EQ( comp.logged(), false ); // id 0, nspecies 0
//   comp.ID++;
//   EXPECT_EQ( comp.logged(), false ); // id > 0, nspecies 0
//   comp.ID--;
//   init();
//   EXPECT_EQ( comp.logged(), false ); // id 0, nspecies > 0
//   comp.ID++;
//   EXPECT_EQ( comp.logged(), true ); // id > 0, ncpecies > 0
// }

// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// TEST_F(IsotopicDefsTests, parent) { 
//   IsoVectorPtr ptest_rent = IsoVectorPtr(new IsoVector);
//   comp.setParent(ptest_rent);
//   EXPECT_EQ( ptest_rent, comp.parent() );
// }

// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// TEST_F(IsotopicDefsTests, other_constructors) {
//   init();
//   IsoVector test_comp = IsoVector(pfracs); // CompMapPtr constructor 
//   EXPECT_EQ( test_comp, comp );
//   IsoVector test_comp2 = IsoVector(comp); // copy constructor
//   EXPECT_EQ( test_comp2, comp );
//   IsoVector test_comp3;
//   EXPECT_NE( test_comp3, comp );
// }

// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// TEST_F(IsotopicDefsTests, pointers) {
//   init();
//   IsoVectorPtr test_pcomp = IsoVectorPtr(pcomp);
//   EXPECT_EQ( test_pcomp, pcomp );
//   EXPECT_EQ( *test_pcomp, *pcomp );
//   EXPECT_EQ( pcomp->me(), pcomp );
// }

// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// TEST_F(IsotopicDefsTests, normalize) {
//   init();
//   double total1 = comp_sum();
//   comp.normalize();
//   double total2 = comp_sum();
//   EXPECT_NE( total1, total2 );
//   EXPECT_EQ( total2, 1 );
// }
