#include <map>

#include <gtest/gtest.h>

#include "composition.h"
#include "comp_math.h"
#include "env.h"
#include "pyne.h"

using cyclus::Composition;
using cyclus::CompMap;
using pyne::nucname::id;

class TestComp : public Composition {
 public:
  TestComp() {}
  Composition::Chain DecayLine() {
    return *decay_line_.get();
  }
};

TEST(CompositionTests, create_atom) {
  cyclus::Env::SetNucDataPath();

  CompMap v;
  v[922350000] = 2;
  v[922330000] = 1;
  Composition::Ptr c = Composition::CreateFromAtom(v);

  v = c->atom();
  EXPECT_DOUBLE_EQ(v[922350000] / v[922330000], 2 / 1);
  v = c->mass();
  EXPECT_DOUBLE_EQ(v[922350000] / v[922330000],
                   2 * pyne::atomic_mass(922350000) / pyne::atomic_mass(922330000));
}

TEST(CompositionTests, create_mass) {
  cyclus::Env::SetNucDataPath();

  CompMap v;
  v[922350000] = 2;
  v[922330000] = 1;
  Composition::Ptr c = Composition::CreateFromMass(v);

  v = c->mass();
  EXPECT_DOUBLE_EQ(v[922350000] / v[922330000], 2 / 1);
  v = c->atom();
  EXPECT_DOUBLE_EQ(v[922350000] / v[922330000],
                   2 / pyne::atomic_mass(922350000) * pyne::atomic_mass(922330000));
}

TEST(CompositionTests, lineage) {
  cyclus::Env::SetNucDataPath();

  TestComp c;

  int dt = 5;
  Composition::Ptr dec1 = c.Decay(dt);
  Composition::Ptr dec2 = c.Decay(2 * dt);
  Composition::Ptr dec3 = c.Decay(2 * dt);
  Composition::Ptr dec4 = dec1->Decay(2 * dt);
  Composition::Ptr dec5 = dec2->Decay(dt);

  std::map<int, Composition::Ptr> chain = c.DecayLine();

  EXPECT_EQ(chain.size(), 3);
  EXPECT_EQ(chain[dt], dec1);
  EXPECT_EQ(chain[2 * dt], dec2);
  EXPECT_EQ(dec2, dec3);
  EXPECT_EQ(chain[3 * dt], dec4);
  EXPECT_EQ(dec4, dec5);
}

TEST(CompositionTests, decay) {
  cyclus::Env::SetNucDataPath();

  CompMap v;
  v[id("Cs137")] = 1;
  v[id("U238")] = 10;
  cyclus::compmath::Normalize(&v);
  Composition::Ptr c = Composition::CreateFromAtom(v);

  double secs_per_timestep = 2419200.0;
  Composition::Ptr newc = c->Decay(int(pyne::half_life("Cs137") / secs_per_timestep));

  CompMap newv = newc->atom();
  cyclus::compmath::Normalize(&newv);

  ASSERT_TRUE(newv.size() > 0) << "decayed composition has zero nuclides - what?";
  ASSERT_TRUE(newv.size() > v.size()) << "decayed composition should have more nuclides than original";
  EXPECT_NEAR(v[id("Cs137")] / 2, newv[id("Cs137")], 1e-4);
  EXPECT_NEAR(v[id("U238")], newv[id("U238")], 1e-4);
}

