
#include "gtest/gtest.h"

#include "comp_math.h"
#include "composition.h"
#include "cyc_limits.h"
#include "error.h"

namespace cm = cyclus::compmath;
using cyclus::Composition;
using cyclus::CompMap;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTest, SubSame) {
  CompMap v;
  v[92235] = 1;
  v[92238] = 2;

  v = cm::Sub(v, v);

  for (CompMap::iterator it = v.begin(); it != v.end(); ++it) {
    EXPECT_DOUBLE_EQ(0, it->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTest, SubCloseSize) {
  CompMap v;
  double qty = 0.1;
  v[92235] = qty;

  CompMap v2(v);
  cm::Normalize(&v2, qty - cyclus::eps_rsrc());

  CompMap remainder;

  remainder = cm::Sub(v, v2);
  CompMap::iterator it;
  for (it = remainder.begin(); it != remainder.end(); ++it) {
    double expected = cyclus::eps_rsrc();
    EXPECT_FLOAT_EQ(expected, it->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTest, SubCloseComp) {
  double qty = 0.1;

  CompMap v;
  v[92235] = 1;
  v[92241] = 1;
  v[82208] = 1;
  cm::Normalize(&v, qty);

  CompMap closev(v);
  closev[92241] -= cyclus::eps_rsrc();

  CompMap remainder;
  remainder = cm::Sub(v, closev);

  CompMap::iterator it;
  for (it = remainder.begin(); it != remainder.end(); ++it) {
    double expected = 0;
    if (it->first == 92241) {
      expected = v[92241] - closev[92241];
    }
    EXPECT_FLOAT_EQ(expected, it->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTest, ApplyThresholdZero) {
  // if the threshold is 0, applying the threshold should do nothing
  CompMap v;
  v[1] = 1.0;
  v[2] = 2.0;
  v[3] = 3.0;

  EXPECT_NO_THROW(cm::ApplyThreshold(&v, 0));

  CompMap::iterator it;
  for (it = v.begin(); it != v.end(); ++it) {
    EXPECT_FLOAT_EQ(v[it->first], it->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTest, ApplyThresholdInf) {
  CompMap v;
  v[1] = 1.0;
  v[2] = 2.0;
  v[3] = 3.0;

  // if the threshold is infinit, applying it should zero any vector
  double infty = std::numeric_limits<double>::infinity();
  EXPECT_NO_THROW(cm::ApplyThreshold(&v, infty));

  CompMap::iterator it;
  for (it = v.begin(); it != v.end(); ++it) {
    EXPECT_FLOAT_EQ(0, it->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTest, ApplyThresholdNegative) {
  CompMap v;
  v[1] = 1.0;
  v[2] = 2.0;
  v[3] = 3.0;

  EXPECT_THROW(cm::ApplyThreshold(&v, -1), cyclus::ValueError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTest, ApplyThresholdMedium) {
  CompMap v;
  v[1] = 1.0;
  v[2] = 2.0;
  v[3] = 3.0;

  // if the threshold is in a reasonable range, it should zero small vals
  CompMap::iterator it;
  for (it = v.begin(); it != v.end(); ++it) {
    EXPECT_NO_THROW(cm::ApplyThreshold(&v, it->second));
    EXPECT_FLOAT_EQ(0, v[it->first]);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTest, AlmostEq) {
  CompMap v1;
  v1[1] = 0.1;
  v1[2] = 0.1;
  v1[3] = 0.1;

  CompMap v2(v1);
  CompMap::iterator it;
  for (it = v2.begin(); it != v2.end(); ++it) {
    v2[it->first] = 0.11;
  }

  EXPECT_TRUE(cm::AlmostEq(v1, v2, 0.1));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTest, AlmostEqualZeroEntry) {
  CompMap v1;
  v1[1] = 0.1;
  v1[2] = 0.1;
  v1[3] = 0.1;

  CompMap v2(v1);
  CompMap::iterator it;
  for (it = v1.begin(); it != v1.end(); ++it) {
    v1[it->first] = 0;
  }

  EXPECT_TRUE(cm::AlmostEq(v1, v2, 1.1));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTest, AlmostEqualNegThresh) {
  CompMap v1, v2;
  EXPECT_THROW(cm::AlmostEq(v1, v2, -1.0), cyclus::ValueError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTests, SimpleAdd) {
  CompMap v1;
  v1[1] = 1.1;
  v1[2] = 2.2;
  v1[3] = 3.3;
  cm::Normalize(&v1, 1.0);
  CompMap v2(v1);
  cm::Normalize(&v2, 2.0);

  CompMap result = cm::Add(v1, v2);
  cm::Normalize(&result);
  EXPECT_TRUE(cm::AlmostEq(v1, result, 1e-15));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTests, Add) {
  CompMap v1;
  v1[1] = 1.0;
  v1[2] = 2.0;
  v1[3] = 3.0;


  CompMap v2;
  v2[1] = 1.1;
  v2[2] = 2.2;
  v2[3] = 3.3;

  CompMap expect;
  expect[1] = 2.1;
  expect[2] = 4.2;
  expect[3] = 6.3;

  CompMap result;

  result = cm::Add(v1, v2);
  EXPECT_TRUE(cm::AlmostEq(result, expect, 0));
  for (CompMap::iterator it = result.begin();
       it != result.end(); it++) {
    EXPECT_DOUBLE_EQ(it->second, expect[it->first]);
  }
}

