// material_tests.h

#include <gtest/gtest.h>

#include "material.h"
#include "composition.h"
#include "timer.h"
#include "error.h"

namespace units = cyclus::units;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MaterialTest : public ::testing::Test {
 protected:
  cyclus::Iso u235_, am241_, th228_, pb208_, pu239_;
  int one_g_; // grams
  cyclus::Composition::Ptr test_comp_, diff_comp_;
  double test_size_, fraction;
  cyclus::Material::Ptr test_mat_, two_test_mat_, ten_test_mat_;
  cyclus::Material::Ptr diff_mat_;
  cyclus::Material::Ptr default_mat_;
  long int u235_halflife_;
  int th228_halflife_;
  double u235_g_per_mol_;

  virtual void SetUp() {
    // composition set up
    u235_ = 92235;
    am241_ = 95241;
    th228_ = 90228;
    pb208_ = 82208;
    test_size_ = 10 * units::g;
    fraction = 2.0 / 3.0;

    // composition creation
    cyclus::Composition::Vect v;
    v[u235_] = 1;
    test_comp_ = cyclus::Composition::CreateFromMass(v);

    v[u235_] = 1;
    v[pb208_] = 1;
    v[am241_] = 1;
    diff_comp_ = cyclus::Composition::CreateFromMass(v);

    default_mat_ = cyclus::Material::Create(0 * units::g, test_comp_);
    test_mat_ = cyclus::Material::Create(test_size_, test_comp_);
    two_test_mat_ = cyclus::Material::Create(2 * test_size_, test_comp_);
    ten_test_mat_ = cyclus::Material::Create(10 * test_size_, test_comp_);
    diff_mat_ = cyclus::Material::Create(test_size_, diff_comp_);

    // test info
    u235_g_per_mol_ = 235.044;
    u235_halflife_ = 8445600000; // approximate, in months
    th228_halflife_ = 2 * 11; // approximate, in months
    int time_ = cyclus::TI->time();
  }

  virtual void TearDown() { }
};
