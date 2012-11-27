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
  TestCompMap comp_;
  Map map_, atomified_, massified_;
  Basis basis_;
  double ratio_;
  int n_species_;
  std::vector<int> isotopes_, masses_;

  boost::shared_ptr<TestCompMap> child, parent;
  CompMapPtr root;
  int t1, t2, root_decay_time;

 public:
  virtual void SetUp() { 
    basis_ = MASS;
    map_ = Map();
    comp_ = TestCompMap(basis_);
    isotopes_.push_back(1001),isotopes_.push_back(2004);
    masses_.push_back(10),masses_.push_back(20);
  }

  virtual void TearDown() {}

  void LoadMap() {
    n_species_ = isotopes_.size();
    double mass_sum, atom_sum;
    for (int i = 0; i < n_species_; i++) {
      int iso = isotopes_.at(i);
      double mass_value = (double)masses_.at(i);
      double atom_value = mass_value / MT->gramsPerMol(iso);
      map_[iso] = mass_value;
      massified_[iso] = mass_value;
      atomified_[iso] = atom_value;
      mass_sum += mass_value;
      atom_sum += atom_value;
    }
    ratio_ = mass_sum/atom_sum;
    for (Map::iterator it = atomified_.begin(); it != atomified_.end(); it++) {
      massified_[it->first] /= mass_sum;
      atomified_[it->first] /= atom_sum;
    }
  }

  void LoadLineage() {
    t1 = 3;
    t2 = 1001;
    // root
    root_decay_time = t1+t2;
    root = CompMapPtr(new CompMap(basis_));
    // parent/
    parent = boost::shared_ptr<TestCompMap>(new TestCompMap(basis_));
    parent->setParent(root);
    parent->setDecayTime(t1);
    // child
    child = boost::shared_ptr<TestCompMap>(new TestCompMap(basis_));
    child->setParent(parent->me());
    child->setDecayTime(t2);
  }
};

