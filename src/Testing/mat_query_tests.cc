// mat_query_tests.cc 
#include <gtest/gtest.h>

#include "mat_query.h"
#include "composition.h"
#include "material.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(MatQueryTests, contains) { 
  using cyclus::Composition;
  using cyclus::Material;
  using cyclus::MatQuery;
  namespace units = cyclus::units;

  Composition::Vect v1;
  v1[92235] = 1;
  v1[92238] = 9;
  v1[1001] = 90;
  Composition::Ptr c1 = Composition::CreateFromMass(v1);
  Material::Ptr m(Material::Create(1 * units::kg, c1));

  Composition::Vect v2;
  v2[92235] = 1;
  v2[92238] = 1;
  v2[1001] = 1;
  Composition::Ptr c2 = Composition::CreateFromMass(v2);

  Composition::Vect v3;
  v3[92235] = 1;
  v3[92238] = 9;
  v3[1001] = 91;
  Composition::Ptr c3 = Composition::CreateFromMass(v3);

  MatQuery mq(m);
  EXPECT_DOUBLE_EQ(mq.Contains(c2), .01 / (1.0 / 3.0));
  EXPECT_DOUBLE_EQ(mq.Contains(c3), .90 / (91.0 / 101.0));

  m->ExtractComp(mq.Contains(c3), c3);
  EXPECT_DOUBLE_EQ(mq.mass(1001), 0);
  EXPECT_TRUE(mq.mass(92235) > 0);
  EXPECT_TRUE(mq.mass(92238) > 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(MatQueryTests, massFrac) { 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(MatQueryTests, atomFrac) { 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(MatQueryTests, mass) { 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(MatQueryTests, moles) { 
}

