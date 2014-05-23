// mat_query_tests.cc
#include <gtest/gtest.h>

#include "composition.h"
#include "context.h"
#include "env.h"
#include "material.h"
#include "pyne.h"
#include "recorder.h"
#include "timer.h"
#include "toolkit/mat_query.h"

namespace cyclus {
namespace toolkit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(MatQueryTests, MassAndMoles) {
  CompMap v;
  Env::SetNucDataPath();

  v[922350000] = 1.5;
  v[10070000] = 2.5;
  Composition::Ptr c = Composition::CreateFromMass(v);
  Material::Ptr m = Material::CreateUntracked(4.0, c);

  MatQuery mq(m);

  EXPECT_DOUBLE_EQ(mq.mass(922350000), 1.5);
  EXPECT_DOUBLE_EQ(mq.mass(10070000), 2.5);
  EXPECT_DOUBLE_EQ(mq.moles(922350000), 1500 / pyne::atomic_mass(922350000));
  EXPECT_DOUBLE_EQ(mq.moles(10070000), 2500 / pyne::atomic_mass(10070000));
  EXPECT_DOUBLE_EQ(mq.mass_frac(922350000), 1.5 / 4.0);
  EXPECT_DOUBLE_EQ(mq.mass_frac(10070000), 2.5 / 4.0);
  double nmoles = mq.moles(922350000) + mq.moles(10070000);
  EXPECT_DOUBLE_EQ(mq.atom_frac(922350000), 1500 / pyne::atomic_mass(922350000) / nmoles);
  EXPECT_DOUBLE_EQ(mq.atom_frac(10070000), 2500 / pyne::atomic_mass(10070000) / nmoles);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(MatQueryTests, AlmostEq) {
  CompMap v;
  Composition::Ptr c;

  v[922350000] = 1.5;
  v[10070000] = 2.5;
  c = Composition::CreateFromMass(v);
  Material::Ptr m1 = Material::CreateUntracked(4.0, c);
  MatQuery mq(m1);

  Material::Ptr m2 = Material::CreateUntracked(4.0, c);
  EXPECT_TRUE(mq.AlmostEq(m2, 0));

  c = Composition::CreateFromMass(v);
  Material::Ptr m3 = Material::CreateUntracked(4.0, c);
  EXPECT_TRUE(mq.AlmostEq(m3, 0));

  v[10070000] += 0.99 * eps_rsrc();
  c = Composition::CreateFromMass(v);
  Material::Ptr m4 = Material::CreateUntracked(4.0, c);
  EXPECT_FALSE(mq.AlmostEq(m4, 0));
  EXPECT_TRUE(mq.AlmostEq(m4, eps_rsrc()));

  v[10070000] += 4.0 * eps_rsrc();
  c = Composition::CreateFromMass(v);
  Material::Ptr m5 = Material::CreateUntracked(4.0, c);
  EXPECT_FALSE(mq.AlmostEq(m5, eps_rsrc()));
  EXPECT_TRUE(mq.AlmostEq(m5, 4.0 * eps_rsrc()));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(MatQueryTests, Amount) {
  Env::SetNucDataPath();

  CompMap v;
  CompMap v2;
  CompMap v3;
  CompMap v4;
  CompMap v5;

  double qty = 4;
  v[922350000] = 1.5;
  v[10070000] = 2.5;
  v2[922350000] = 1;
  v3[10070000] = 1;
  v4[922350000] = 1;
  v4[10070000] = 10;
  v5[942490000] = 1;

  Composition::Ptr c = Composition::CreateFromMass(v);
  Composition::Ptr c2 = Composition::CreateFromMass(v2);
  Composition::Ptr c3 = Composition::CreateFromMass(v3);
  Composition::Ptr c4 = Composition::CreateFromMass(v4);
  Composition::Ptr c5 = Composition::CreateFromMass(v5);
  Material::Ptr m = Material::CreateUntracked(qty, c);
  MatQuery mq(m);

  EXPECT_DOUBLE_EQ(1.5, mq.Amount(c2));
  EXPECT_DOUBLE_EQ(2.5, mq.Amount(c3));
  EXPECT_DOUBLE_EQ(2.75, mq.Amount(c4));
  EXPECT_DOUBLE_EQ(0, mq.Amount(c5));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MaterialMatQueryTest : public ::testing::Test {
 protected:
  Nuc u235_, am241_, th228_, pb208_, pu239_;
  int one_g_;  // grams
  Composition::Ptr test_comp_, diff_comp_;
  double test_size_, fraction;
  Material::Ptr test_mat_, two_test_mat_, ten_test_mat_;
  Material::Ptr diff_mat_;
  Material::Ptr default_mat_;
  long int u235_halflife_;
  int th228_halflife_;
  double u235_g_per_mol_;

  virtual void SetUp() {
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

    // test info
    u235_g_per_mol_ = 235.044;
    u235_halflife_ = 8445600000;  // approximate, in months
    th228_halflife_ = 2 * 11;  // approximate, in months
  }

  virtual void TearDown() {}
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MaterialMatQueryTest, ExtractCompleteInexactComp) {
  // Complete extraction
  // this should succeed even if inexact, within eps.
  Material::Ptr m1;
  // make an inexact composition

  CompMap inexact = diff_comp_->mass();
  inexact[am241_] *= (1 - eps_rsrc() / test_size_);
  Composition::Ptr inexact_comp = Composition::CreateFromMass(inexact);

  m1 = diff_mat_->ExtractComp(test_size_, inexact_comp);
  EXPECT_EQ(m1->comp(), inexact_comp);
  EXPECT_NEAR(test_size_, m1->quantity(), eps_rsrc());

  MatQuery mq(diff_mat_);
  EXPECT_NEAR(0, mq.mass(am241_), eps_rsrc());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MaterialMatQueryTest, ExtractDiffComp) {
  // differing comp minus one element equals old comp minus new
  Material::Ptr m1;
  Env::SetNucDataPath();
  double orig = diff_mat_->quantity();
  CompMap v = diff_mat_->comp()->atom();

  MatQuery mq(diff_mat_);
  const double orig_u235 = mq.mass(u235_);
  const double orig_am241 = mq.mass(am241_);
  const double orig_pb208 = mq.mass(pb208_);

  EXPECT_NO_THROW(m1 = diff_mat_->ExtractComp(mq.mass(u235_), test_comp_));
  EXPECT_DOUBLE_EQ(orig - m1->quantity(), diff_mat_->quantity());
  EXPECT_EQ(m1->comp(), test_comp_);
  EXPECT_NE(diff_mat_->comp(), test_comp_);
  EXPECT_DOUBLE_EQ(orig_am241, mq.mass(am241_));
  EXPECT_DOUBLE_EQ(orig_pb208, mq.mass(pb208_));
  EXPECT_NE(orig_u235, mq.mass(u235_));
}

} // namespace toolkit
} // namespace cyclus
