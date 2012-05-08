// CompMapTests.h
#include <gtest/gtest.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/pointer_cast.hpp>

#include "CompMap.h"

#include "MassTable.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class TestCompMap : public CompMap {
 public:
 TestCompMap() : CompMap(MASS) {};
 TestCompMap(Basis b) : CompMap(b) {};
  
  virtual ~TestCompMap() {};
  
  void setMap(Map m) {map_ = m;}
  void setParent(CompMapPtr p) {parent_ = p;}
  void setDecayTime(double t) {decay_time_ = t;}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CompMapTests : public ::testing::Test {
 protected:
  TestCompMap comp;
  Map map, atomified, massified;
  Basis basis;
  double ratio;
  int nSpecies;
  std::vector<int> isotopes, masses;

  boost::shared_ptr<TestCompMap> child, parent;
  CompMapPtr root;
  double t1, t2, root_decay_time;

 public:
  virtual void SetUp() { 
    basis = MASS;
    map = Map();
    comp = TestCompMap(basis);
    isotopes.push_back(1001),isotopes.push_back(2004);
    masses.push_back(10),masses.push_back(20);
  }

  virtual void TearDown() {}

  void LoadMap() {
    nSpecies = isotopes.size();
    double mass_sum, atom_sum;
    for (int i = 0; i < nSpecies; i++) {
      int iso = isotopes.at(i);
      double mass_value = (double)masses.at(i);
      double atom_value = mass_value / MT->gramsPerMol(iso);
      map[iso] = mass_value;
      massified[iso] = mass_value;
      atomified[iso] = atom_value;
      mass_sum += mass_value;
      atom_sum += atom_value;
    }
    ratio = mass_sum/atom_sum;
    for (Map::iterator it = atomified.begin(); it != atomified.end(); it++) {
      massified[it->first] /= mass_sum;
      atomified[it->first] /= atom_sum;
    }
  }

  void LoadLineage() {
    t1 = 1.5;
    t2 = 1000.71;
    // root
    root_decay_time = t1+t2;
    root = CompMapPtr(new CompMap(basis));
    // parent/
    parent = boost::shared_ptr<TestCompMap>(new TestCompMap(basis));
    parent->setParent(root);
    parent->setDecayTime(t1);
    // child
    child = boost::shared_ptr<TestCompMap>(new TestCompMap(basis));
    child->setParent(parent->me());
    child->setDecayTime(t2);
  }
};

