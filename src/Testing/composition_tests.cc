// CompositionTests.cpp
#include <map>
#include <gtest/gtest.h>

#include "composition.h"
#include "mass_table.h"

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

  Composition::Vect v;
  v[92235] = 2;
  v[92233] = 1;
  Composition::Ptr c = Composition::CreateFromAtom(v);

  v = c->atom_vect();
  EXPECT_DOUBLE_EQ(v[92235] / v[92233], 2 / 1);
  v = c->mass_vect();
  EXPECT_DOUBLE_EQ(v[92235] / v[92233], 2 * cyclus::MT->GramsPerMol(92235) / cyclus::MT->GramsPerMol(92233));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompositionTests, create_mass) {
  using cyclus::Composition;

  Composition::Vect v;
  v[92235] = 2;
  v[92233] = 1;
  Composition::Ptr c = Composition::CreateFromMass(v);

  v = c->mass_vect();
  EXPECT_DOUBLE_EQ(v[92235] / v[92233], 2 / 1);
  v = c->atom_vect();
  EXPECT_DOUBLE_EQ(v[92235] / v[92233], 2 / cyclus::MT->GramsPerMol(92235) * cyclus::MT->GramsPerMol(92233));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompositionTests, lineage) {
  using cyclus::Composition;

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

