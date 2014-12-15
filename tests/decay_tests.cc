#include <gtest/gtest.h>

#include "comp_math.h"
#include "composition.h"
#include "env.h"
#include "pyne.h"
#include "pyne_decay.h"

using pyne::nucname::id;

TEST(DecayTests, simple) {
  using cyclus::CompMap;
  cyclus::Env::SetNucDataPath();

  CompMap v;
  v[id("Cs137")] = 1;
  v[id("U238")] = 10;
  cyclus::compmath::Normalize(&v);

  CompMap newv = pyne::decayers::decay(v, pyne::half_life("Cs137"));
  cyclus::compmath::Normalize(&newv);

  ASSERT_TRUE(newv.size() > 0) << "decayed composition has zero nuclides - what?";
  ASSERT_TRUE(newv.size() > v.size()) << "decayed composition should have more nuclides than original";
  EXPECT_NEAR(v[id("Cs137")] / 2, newv[id("Cs137")], 1e-6);
  EXPECT_NEAR(v[id("U238")], newv[id("U238")], 1e-6);
}

