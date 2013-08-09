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
  cyclus::Composition::Vect v;
  v[92235] = 2;
  v[92233] = 1;
  Composition::Ptr c = cyclus::Composition::CreateFromAtom(v);

  v = c->atom_vect();
  EXPECT_DOUBLE_EQ(v[92235] / v[92233], 2 / 1);
  v = c->mass_vect();
  EXPECT_DOUBLE_EQ(v[92235] / v[92233], 2 * MT->GramsPerMol(92235) / MT->GramsPerMol(92233));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompositionTests, create_mass) {
  cyclus::Composition::Vect v;
  v[92235] = 2;
  v[92233] = 1;
  Composition::Ptr c = cyclus::Composition::CreateFromMass(v);

  v = c->mass_vect();
  EXPECT_DOUBLE_EQ(v[92235] / v[92233], 2 / 1);
  v = c->atom_vect();
  EXPECT_DOUBLE_EQ(v[92235] / v[92233], 2 / MT->GramsPerMol(92235) * MT->GramsPerMol(92233));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompositionTests, lineage) {
  TestComp c;

  int dt = 5;
  cyclus::Composition::Ptr dec1 = c.Decay(dt);
  cyclus::Composition::Ptr dec2 = c.Decay(2 * dt);
  cyclus::Composition::Ptr dec3 = c.Decay(2 * dt);
  cyclus::Composition::Ptr dec4 = dec1->Decay(2 * dt);
  cyclus::Composition::Ptr dec5 = dec2->Decay(dt);

  std::map<int, cyclus::Composition::Ptr> chain = c.DecayLine();

  EXPECT_EQ(chain.size(), 3);
  EXPECT_EQ(chain[dt], dec1);
  EXPECT_EQ(chain[2 * dt], dec2);
  EXPECT_EQ(dec2, dec3);
  EXPECT_EQ(chain[3 * dt], dec4);
  EXPECT_EQ(dec4, dec5);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CompositionTests, decay) {
  /// \@MJG_FLAG this needs to be written... think about the best way to do it
}
