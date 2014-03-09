// mat_query_tests.cc
#include <gtest/gtest.h>

#include "composition.h"
#include "context.h"
#include "env.h"
#include "mat_query.h"
#include "material.h"
#include "pyne.h"
#include "recorder.h"
#include "timer.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(MatQueryTests, MassAndMoles) { 
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &rec);
  cyclus::CompMap v;
  cyclus::Env::SetNucDataPath();

  v[922350000] = 1.5;
  v[10070000] = 2.5;
  cyclus::Composition::Ptr c = cyclus::Composition::CreateFromMass(v);
  cyclus::Material::Ptr m = cyclus::Material::CreateUntracked(4.0, c);

  cyclus::MatQuery mq(m);

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(MatQueryTests, AlmostEq) { 
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &rec);
  cyclus::CompMap v;
  cyclus::Composition::Ptr c;

  v[922350000] = 1.5;
  v[10070000] = 2.5;
  c = cyclus::Composition::CreateFromMass(v);
  cyclus::Material::Ptr m1 = cyclus::Material::CreateUntracked(4.0, c);
  cyclus::MatQuery mq(m1);

  cyclus::Material::Ptr m2 = cyclus::Material::CreateUntracked(4.0, c);
  EXPECT_TRUE(mq.AlmostEq(m2, 0));

  c = cyclus::Composition::CreateFromMass(v);
  cyclus::Material::Ptr m3 = cyclus::Material::CreateUntracked(4.0, c);
  EXPECT_TRUE(mq.AlmostEq(m3, 0));

  v[10070000] += 0.99 * cyclus::eps_rsrc();
  c = cyclus::Composition::CreateFromMass(v);
  cyclus::Material::Ptr m4 = cyclus::Material::CreateUntracked(4.0, c);
  EXPECT_FALSE(mq.AlmostEq(m4, 0));
  EXPECT_TRUE(mq.AlmostEq(m4, cyclus::eps_rsrc()));

  v[10070000] += 4.0 * cyclus::eps_rsrc();
  c = cyclus::Composition::CreateFromMass(v);
  cyclus::Material::Ptr m5 = cyclus::Material::CreateUntracked(4.0, c);
  EXPECT_FALSE(mq.AlmostEq(m5, cyclus::eps_rsrc()));
  EXPECT_TRUE(mq.AlmostEq(m5, 4.0 * cyclus::eps_rsrc()));
}

