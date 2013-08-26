
#include "gtest/gtest.h"

#include "comp_math.h"
#include "composition.h"
#include "cyc_limits.h"
#include "error.h"

namespace cm = cyclus::compmath;
using cyclus::Composition;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTest, SubSame) {
  Composition::Vect v;
  v[92235] = 1;
  v[92238] = 2;

  v = cm::Sub(v, v);

  for (Composition::Vect::iterator it = v.begin(); it != v.end(); ++it) {
    EXPECT_DOUBLE_EQ(0, it->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTest, SubCloseSize) {
  Composition::Vect v;
  double qty = 0.1;
  v[92235] = qty;

  Composition::Vect v2(v);
  cm::Normalize(&v2, qty - cyclus::eps_rsrc());

  Composition::Vect remainder;

  remainder = cm::Sub(v, v2);
  Composition::Vect::iterator it;
  for (it = remainder.begin(); it != remainder.end(); ++it) {
    double expected = cyclus::eps_rsrc();
    EXPECT_FLOAT_EQ(expected, it->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTest, SubCloseComp) {
  double qty = 0.1;

  Composition::Vect v;
  v[92235] = 1;
  v[92241] = 1;
  v[82208] = 1;
  cm::Normalize(&v, qty);

  Composition::Vect closev(v);
  closev[92241] -= cyclus::eps_rsrc();

  Composition::Vect remainder;
  remainder = cm::Sub(v, closev);

  Composition::Vect::iterator it;
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
  Composition::Vect v;
  v[1] = 1.0;
  v[2] = 2.0;
  v[3] = 3.0;

  EXPECT_NO_THROW(cm::ApplyThreshold(&v, 0));

  Composition::Vect::iterator it;
  for (it = v.begin(); it != v.end(); ++it) {
    EXPECT_FLOAT_EQ(v[it->first], it->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTest, ApplyThresholdInf) {
  Composition::Vect v;
  v[1] = 1.0;
  v[2] = 2.0;
  v[3] = 3.0;

  // if the threshold is infinit, applying it should zero any vector
  double infty = std::numeric_limits<double>::infinity();
  EXPECT_NO_THROW(cm::ApplyThreshold(&v, infty));

  Composition::Vect::iterator it;
  for (it = v.begin(); it != v.end(); ++it) {
    EXPECT_FLOAT_EQ(0, it->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTest, ApplyThresholdNegative) {
  Composition::Vect v;
  v[1] = 1.0;
  v[2] = 2.0;
  v[3] = 3.0;

  EXPECT_THROW(cm::ApplyThreshold(&v, -1), cyclus::ValueError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTest, ApplyThresholdMedium) {
  Composition::Vect v;
  v[1] = 1.0;
  v[2] = 2.0;
  v[3] = 3.0;

  // if the threshold is in a reasonable range, it should zero small vals
  Composition::Vect::iterator it;
  for (it = v.begin(); it != v.end(); ++it) {
    EXPECT_NO_THROW(cm::ApplyThreshold(&v, it->second));
    EXPECT_FLOAT_EQ(0, v[it->first]);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTest, AlmostEquality) {
  Composition::Vect v1;
  v1[1] = 0.1;
  v1[2] = 0.1;
  v1[3] = 0.1;

  Composition::Vect v2(v1);
  Composition::Vect::iterator it;
  for (it = v2.begin(); it != v2.end(); ++it) {
    v2[it->first] *= 1.1;
  }

  EXPECT_TRUE(cm::AlmostEq(v1, v2, 0.1));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTest, AlmostEqualZeroEntry) {
  Composition::Vect v1;
  v1[1] = 0.1;
  v1[2] = 0.1;
  v1[3] = 0.1;

  Composition::Vect v2(v1);
  Composition::Vect::iterator it;
  for (it = v1.begin(); it != v1.end(); ++it) {
    v1[it->first] = 0;
  }

  EXPECT_TRUE(cm::AlmostEq(v1, v2, 1.1));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTest, AlmostEqualNegThresh) {
  Composition::Vect v1, v2;
  EXPECT_THROW(cm::AlmostEq(v1, v2, -1.0), cyclus::ValueError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTests, SimpleAdd) {
  Composition::Vect v1;
  v1[1] = 1.1;
  v1[2] = 2.2;
  v1[3] = 3.3;
  cm::Normalize(&v1, 1.0);
  Composition::Vect v2(v1);
  cm::Normalize(&v2, 2.0);

  Composition::Vect result = cm::Add(v1, v2);
  cm::Normalize(&result);
  EXPECT_TRUE(cm::AlmostEq(v1, result, 1e-15));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompMathTests, Add) {
  Composition::Vect v1;
  v1[1] = 1.0;
  v1[2] = 2.0;
  v1[3] = 3.0;


  Composition::Vect v2;
  v2[1] = 1.1;
  v2[2] = 2.2;
  v2[3] = 3.3;

  Composition::Vect expect;
  expect[1] = 2.1;
  expect[2] = 4.2;
  expect[3] = 6.3;

  Composition::Vect result;

  result = cm::Add(v1, v2);
  EXPECT_TRUE(cm::AlmostEq(result, expect, 0));
  for (Composition::Vect::iterator it = result.begin();
       it != result.end(); it++) {
    EXPECT_DOUBLE_EQ(it->second, expect[it->first]);
  }
}

