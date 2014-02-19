// material_tests.cpp
#include "material_tests.h"

#include <cmath>
#include <gtest/gtest.h>

#include "cyc_limits.h"
#include "error.h"
#include "mat_query.h"

using cyclus::Nuc;
using cyclus::CompMap;
using cyclus::Composition;
using cyclus::Material;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MaterialTest, Constructors) {
  EXPECT_EQ(default_mat_->units(), "kg");
  EXPECT_EQ(default_mat_->type(), cyclus::Material::kType);
  EXPECT_GE(default_mat_->id(), 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MaterialTest, Clone) {
  cyclus::Resource::Ptr clone_mat;
  ASSERT_NO_THROW(clone_mat = test_mat_->Clone());

  EXPECT_EQ(test_mat_->type(), clone_mat->type());
  EXPECT_EQ(test_mat_->units(), clone_mat->units());
  EXPECT_DOUBLE_EQ(test_mat_->quantity(), clone_mat->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MaterialTest, ExtractRes) {
  EXPECT_DOUBLE_EQ(test_size_, test_mat_->quantity());
  double other_size = test_size_ / 3;
  cyclus::Resource::Ptr other;
  ASSERT_NO_THROW(other = test_mat_->ExtractRes(other_size));
  EXPECT_DOUBLE_EQ(test_size_ - other_size, test_mat_->quantity());
  EXPECT_DOUBLE_EQ(other_size, other->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MaterialTest, SimpleAbsorb) {
  double val = 1.5 * units::kg;
  cyclus::Material::Ptr m1 = cyclus::Material::CreateUntracked(val, test_comp_);
  cyclus::Material::Ptr m2 = cyclus::Material::CreateUntracked(val, test_comp_);
  ASSERT_EQ(m1->comp(), m2->comp());
  ASSERT_EQ(m1->quantity(), m2->quantity());

  m2->Absorb(m1);

  ///ASSERT_EQ(m1->comp(), m2->comp());
  ///EXPECT_EQ(m1->quantity(), 0);
  ///EXPECT_EQ(m2->quantity(), 2 * val);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MaterialTest, AbsorbLikeMaterial) {
  cyclus::Material::Ptr mat1;
  cyclus::Material::Ptr mat2;
  cyclus::Material::Ptr mat10;
  mat1 = cyclus::Material::CreateUntracked(1 * test_size_, test_comp_);
  mat2 = cyclus::Material::CreateUntracked(2 * test_size_, test_comp_);
  mat10 = cyclus::Material::CreateUntracked(10 * test_size_, test_comp_);

  // see that two materials with the same composition do the right thing
  double orig = test_mat_->quantity();
  int factor = 2;
  ASSERT_NO_THROW(test_mat_->Absorb(mat1));
  ASSERT_EQ(test_mat_->comp(), mat1->comp());
  EXPECT_DOUBLE_EQ(test_mat_->quantity(), factor * orig);

  factor += 2;
  ASSERT_NO_THROW(test_mat_->Absorb(mat2));
  ASSERT_EQ(test_mat_->comp(), mat2->comp());
  EXPECT_DOUBLE_EQ(test_mat_->quantity(), factor * orig);

  factor += 10;
  ASSERT_NO_THROW(test_mat_->Absorb(mat10));
  ASSERT_EQ(test_mat_->comp(), mat10->comp());
  EXPECT_DOUBLE_EQ(test_mat_->quantity(), factor * orig);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MaterialTest, AbsorbUnLikeMaterial) {
  // make a number of materials masses 1, 2, and 10
  cyclus::Material::Ptr same_as_orig_test_mat = cyclus::Material::CreateUntracked(0,
                                                test_comp_);

  CompMap v;
  v[pb208_] = 1.0 * units::g;
  v[am241_] = 1.0 * units::g;
  v[th228_] = 1.0 * units::g;
  cyclus::Composition::Ptr diff_test_comp = cyclus::Composition::CreateFromMass(
                                              v);
  cyclus::Material::Ptr diff_test_mat = cyclus::Material::CreateUntracked(
                                        test_size_ / 2,
                                        diff_test_comp);

  double orig = test_mat_->quantity();
  double origdiff = diff_test_mat->quantity();

  // see that materials with different compositions do the right thing
  ASSERT_NO_THROW(test_mat_->Absorb(diff_test_mat));
  EXPECT_NE(test_mat_->comp(), diff_test_mat->comp());
  EXPECT_NE(test_mat_->comp(), same_as_orig_test_mat->comp());
  EXPECT_DOUBLE_EQ(orig + origdiff, test_mat_->quantity());
  EXPECT_TRUE(std::abs(same_as_orig_test_mat->quantity() -
                       test_mat_->quantity()) > cyclus::eps_rsrc());

  // see that an empty material appropriately absorbs a not empty material.
  ASSERT_NO_THROW(default_mat_->Absorb(test_mat_));
  EXPECT_DOUBLE_EQ(orig + origdiff, default_mat_->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MaterialTest, AbsorbZeroMaterial) {
  Material::Ptr same_as_test_mat = Material::CreateUntracked(0, test_comp_);
  EXPECT_NO_THROW(test_mat_->Absorb(same_as_test_mat));
  EXPECT_FLOAT_EQ(test_size_, test_mat_->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MaterialTest, AbsorbIntoZeroMaterial) {
  Material::Ptr same_as_test_mat = Material::CreateUntracked(0, test_comp_);
  EXPECT_NO_THROW(same_as_test_mat->Absorb(test_mat_));
  EXPECT_FLOAT_EQ(test_size_, same_as_test_mat->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MaterialTest, ExtractMass) {
  double amt = test_size_ / 3;
  double diff = test_size_ - amt;
  cyclus::Material::Ptr extracted;
  EXPECT_DOUBLE_EQ(test_mat_->quantity(), test_size_); // we expect this amt
  EXPECT_NO_THROW(extracted = test_mat_->ExtractQty(amt)); // extract an amt
  EXPECT_DOUBLE_EQ(extracted->quantity(), amt); // check correctness
  EXPECT_DOUBLE_EQ(test_mat_->quantity(), diff); // check correctness
  EXPECT_EQ(test_mat_->comp(), extracted->comp());
  EXPECT_THROW(two_test_mat_->ExtractQty(2 * two_test_mat_->quantity()),
               cyclus::Error);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MaterialTest, ExtractComplete) {
  cyclus::Material::Ptr m1;
  EXPECT_NO_THROW(m1 = test_mat_->ExtractComp(test_size_, test_comp_));
  EXPECT_EQ(m1->comp(), test_comp_);
  EXPECT_DOUBLE_EQ(0, test_mat_->quantity());
  EXPECT_DOUBLE_EQ(test_size_, m1->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MaterialTest, ExtractOverQty) {
  EXPECT_THROW(diff_mat_->ExtractComp(2 * test_size_, test_comp_),
               cyclus::ValueError);
  EXPECT_THROW(test_mat_->ExtractComp(2 * test_size_, test_comp_),
               cyclus::ValueError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MaterialTest, ExtractOverComp) {
  Material::Ptr m;

  CompMap inexact = diff_comp_->mass();
  inexact[am241_] *= 2;
  Composition::Ptr inexact_comp = Composition::CreateFromMass(inexact);

  EXPECT_THROW(diff_mat_->ExtractComp(test_size_, inexact_comp),
               cyclus::ValueError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MaterialTest, ExtractCompleteInexactComp) {
  // Complete extraction
  // this should succeed even if inexact, within eps.
  Material::Ptr m1;
  // make an inexact composition

  CompMap inexact = diff_comp_->mass();
  inexact[am241_] *= (1 - cyclus::eps_rsrc() / test_size_);
  Composition::Ptr inexact_comp = Composition::CreateFromMass(inexact);

  m1 = diff_mat_->ExtractComp(test_size_, inexact_comp);
  EXPECT_EQ(m1->comp(), inexact_comp);
  EXPECT_NEAR(test_size_, m1->quantity(), cyclus::eps_rsrc());

  cyclus::MatQuery mq(diff_mat_);
  EXPECT_NEAR(0, mq.mass(am241_), cyclus::eps_rsrc());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MaterialTest, ExtractHalf) {
  cyclus::Material::Ptr m1 = two_test_mat_->ExtractComp(test_size_, test_comp_);
  EXPECT_DOUBLE_EQ(test_size_, m1->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MaterialTest, ExtractDiffComp) {
  // differing comp minus one element equals old comp minus new
  cyclus::Material::Ptr m1;
  double orig = diff_mat_->quantity();
  CompMap v = diff_mat_->comp()->atom();

  cyclus::MatQuery mq(diff_mat_);
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MaterialTest, AbsorbThenExtract) {
  cyclus::Composition::Ptr comp_to_rem = cyclus::Composition::Ptr(test_comp_);
  double kg_to_rem = 0.25 * test_size_;

  // if you start with an empty material
  EXPECT_DOUBLE_EQ(0, default_mat_->quantity());
  // then you absorb another material, they should be identical
  EXPECT_NO_THROW(default_mat_->Absorb(test_mat_));
  EXPECT_EQ(default_mat_->comp(), test_mat_->comp());
  EXPECT_DOUBLE_EQ(test_size_, default_mat_->quantity());
  // and it should be okay to extract a fraction of the original composiiton
  EXPECT_NO_THROW(default_mat_->ExtractComp(kg_to_rem, comp_to_rem));
  EXPECT_DOUBLE_EQ(test_size_ - kg_to_rem, default_mat_->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MaterialTest, ExtractInGrams) {
  cyclus::Composition::Ptr comp_to_rem = cyclus::Composition::Ptr(test_comp_);
  double kg_to_rem = 0.25 * test_size_;
  double g_to_rem = 1000 * kg_to_rem;

  // if you start with an empty material
  EXPECT_DOUBLE_EQ(0, default_mat_->quantity());
  // then you absorb another material, they should be identical
  EXPECT_NO_THROW(default_mat_->Absorb(test_mat_));
  EXPECT_EQ(default_mat_->comp(), test_mat_->comp());
  EXPECT_DOUBLE_EQ(test_size_, default_mat_->quantity());
  // and it should be okay to extract part of the original composiiton IN GRAMS
  EXPECT_NO_THROW(default_mat_->ExtractComp(g_to_rem * units::g, comp_to_rem));
  EXPECT_DOUBLE_EQ(test_size_ - kg_to_rem, default_mat_->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MaterialTest, DecayShortcut) {
  using cyclus::Composition;
  cyclus::CompMap mp;
  mp[922350000] = 1;
  Composition::Ptr c = Composition::CreateFromAtom(mp);
  cyclus::Material::Ptr m = Material::CreateUntracked(1.0, c);

  double u235_decay_const = 8.087e-11; // per month
  double eps = 1e-3;
  double threshold = -1 * std::log(1-eps) / u235_decay_const;
  m->Decay(threshold * 0.9);
  EXPECT_EQ(c, m->comp());
}

