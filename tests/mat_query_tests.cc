// mat_query_tests.cc
#include <gtest/gtest.h>

#include "composition.h"
#include "context.h"
#include "event_manager.h"
#include "mass_table.h"
#include "mat_query.h"
#include "material.h"
#include "timer.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(MatQueryTests, MassAndMoles) { 
  cyclus::EventManager em;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &em);
  cyclus::MassTable* mt = cyclus::MT;

  cyclus::CompMap v;
  v[92235] = 1.5;
  v[1008] = 2.5;
  cyclus::Composition::Ptr c = cyclus::Composition::CreateFromMass(v);
  cyclus::Material::Ptr m = cyclus::Material::Create(&ctx, 4.0, c);

  cyclus::MatQuery mq(m);

  EXPECT_DOUBLE_EQ(mq.mass(92235), 1.5);
  EXPECT_DOUBLE_EQ(mq.mass(1008), 2.5);
  EXPECT_DOUBLE_EQ(mq.moles(92235), 1500 / mt->GramsPerMol(92235));
  EXPECT_DOUBLE_EQ(mq.moles(1008), 2500 / mt->GramsPerMol(1008));
  EXPECT_DOUBLE_EQ(mq.mass_frac(92235), 1.5 / 4.0);
  EXPECT_DOUBLE_EQ(mq.mass_frac(1008), 2.5 / 4.0);
  double nmoles = mq.moles(92235) + mq.moles(1008);
  EXPECT_DOUBLE_EQ(mq.atom_frac(92235), 1500 / mt->GramsPerMol(92235) / nmoles);
  EXPECT_DOUBLE_EQ(mq.atom_frac(1008), 2500 / mt->GramsPerMol(1008) / nmoles);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(MatQueryTests, AlmostEq) { 
  cyclus::EventManager em;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &em);
  cyclus::CompMap v;
  cyclus::Composition::Ptr c;

  v[92235] = 1.5;
  v[1008] = 2.5;
  c = cyclus::Composition::CreateFromMass(v);
  cyclus::Material::Ptr m1 = cyclus::Material::Create(&ctx, 4.0, c);
  cyclus::MatQuery mq(m1);

  cyclus::Material::Ptr m2 = cyclus::Material::Create(&ctx, 4.0, c);
  EXPECT_TRUE(mq.AlmostEq(m2, 0));

  c = cyclus::Composition::CreateFromMass(v);
  cyclus::Material::Ptr m3 = cyclus::Material::Create(&ctx, 4.0, c);
  EXPECT_TRUE(mq.AlmostEq(m3, 0));

  v[1008] += 0.99 * cyclus::eps_rsrc();
  c = cyclus::Composition::CreateFromMass(v);
  cyclus::Material::Ptr m4 = cyclus::Material::Create(&ctx, 4.0, c);
  EXPECT_FALSE(mq.AlmostEq(m4, 0));
  EXPECT_TRUE(mq.AlmostEq(m4, cyclus::eps_rsrc()));

  v[1008] += 4.0 * cyclus::eps_rsrc();
  c = cyclus::Composition::CreateFromMass(v);
  cyclus::Material::Ptr m5 = cyclus::Material::Create(&ctx, 4.0, c);
  EXPECT_FALSE(mq.AlmostEq(m5, cyclus::eps_rsrc()));
  EXPECT_TRUE(mq.AlmostEq(m5, 4.0 * cyclus::eps_rsrc()));
}

