#ifndef CYCLUS_TESTS_MATERIAL_TESTS_H_
#define CYCLUS_TESTS_MATERIAL_TESTS_H_

#include <gtest/gtest.h>

#include "composition.h"
#include "material.h"
#include "env.h"
#include "context.h"
#include "test_agents/test_facility.h"
#include "recorder.h"
#include "timer.h"


namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MaterialTest : public ::testing::Test {
 protected:
  Nuc u235_, am241_, th228_, pb208_, pu239_;
  int one_g_;  // grams
  Composition::Ptr test_comp_, diff_comp_;
  double test_size_, fraction;
  Material::Ptr test_mat_, two_test_mat_, ten_test_mat_;
  Material::Ptr diff_mat_;
  Material::Ptr default_mat_;
  Material::Ptr tracked_mat_;
  long int u235_halflife_;
  int th228_halflife_;
  double u235_g_per_mol_;

  cyclus::Timer ti;
  cyclus::Recorder rec;
  cyclus::Context* ctx;
  TestFacility* fac;

  virtual void SetUp() {
    ctx = new cyclus::Context(&ti, &rec);
    fac = new TestFacility(ctx);
    Env::SetNucDataPath();

    // composition set up
    u235_ = 922350000;
    am241_ = 952410000;
    th228_ = 902280000;
    pb208_ = 822080000;
    test_size_ = 10 * units::g;
    fraction = 2.0 / 3.0;

    // composition creation
    CompMap v;
    v[u235_] = 1;
    test_comp_ = Composition::CreateFromMass(v);

    v[u235_] = 1;
    v[pb208_] = 1;
    v[am241_] = 1;
    diff_comp_ = Composition::CreateFromMass(v);

    default_mat_ = Material::CreateUntracked(0 * units::g, test_comp_);
    test_mat_ = Material::CreateUntracked(test_size_, test_comp_);
    two_test_mat_ = Material::CreateUntracked(2 * test_size_, test_comp_);
    ten_test_mat_ = Material::CreateUntracked(10 * test_size_, test_comp_);
    diff_mat_ = Material::CreateUntracked(test_size_, diff_comp_);

    // tracked material
    tracked_mat_ = Material::Create(fac, 1000, diff_comp_);

    // test info
    u235_g_per_mol_ = 235.044;
    u235_halflife_ = 8445600000;  // approximate, in months
    th228_halflife_ = 2 * 11;  // approximate, in months
  }

  virtual void TearDown() {
    delete ctx;
  }
};

}  // namespace cyclus

#endif  // CYCLUS_TESTS_MATERIAL_TESTS_H_
