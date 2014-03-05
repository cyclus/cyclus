// CompositionTests.cpp
#include <map>
#include <gtest/gtest.h>

#include "composition.h"
#include "env.h"
#include "mass_table.h"
#include "pyne.h"

class TestComp : public cyclus::Composition {
 public:
  TestComp() {};
  cyclus::Composition::Chain DecayLine() {
    return *decay_line_.get();
  };
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompositionTests, create_atom) {
  using cyclus::Composition;

  cyclus::CompMap v;
  v[922350000] = 2;
  v[922330000] = 1;
  Composition::Ptr c = Composition::CreateFromAtom(v);

  v = c->atom();
  EXPECT_DOUBLE_EQ(v[922350000] / v[922330000], 2 / 1);
  v = c->mass();
  EXPECT_DOUBLE_EQ(v[922350000] / v[922330000], 2 * cyclus::MT->GramsPerMol(922350000) / cyclus::MT->GramsPerMol(922330000));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompositionTests, create_mass) {
  using cyclus::Composition;

  cyclus::CompMap v;
  v[922350000] = 2;
  v[922330000] = 1;
  Composition::Ptr c = Composition::CreateFromMass(v);

  v = c->mass();
  EXPECT_DOUBLE_EQ(v[922350000] / v[922330000], 2 / 1);
  v = c->atom();
  EXPECT_DOUBLE_EQ(v[922350000] / v[922330000], 2 / cyclus::MT->GramsPerMol(922350000) * cyclus::MT->GramsPerMol(922330000));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompositionTests, lineage) {
  using cyclus::Composition;
  // tell pyne about the path to nuc data
  pyne::NUC_DATA_PATH = cyclus::Env::GetBuildPath() + "/share/cyclus_nuc_data.h5";

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
 
