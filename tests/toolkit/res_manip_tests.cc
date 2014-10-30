
#include <gtest/gtest.h>

#include "cyclus.h"

using cyclus::Material;
using cyclus::Product;
using cyclus::Resource;
using cyclus::toolkit::Squash;
using cyclus::toolkit::SquashProd;
using cyclus::toolkit::SquashMat;
using cyclus::toolkit::Separate;


Material::Ptr testmat() {
  cyclus::CompMap cm;
  cm[922350000] = 2;
  cm[922380000] = 4;
  cm[94239] = 1;
  cyclus::Composition::Ptr c = cyclus::Composition::CreateFromMass(cm);
  return Material::CreateUntracked(8, c);
}

Product::Ptr testprod() {
  return Product::CreateUntracked(8, "bananas");
}

TEST(ResManipTests, SquashMat) {
  std::vector<Material::Ptr> mats;
  int n = 3;
  for (int i = 0; i < n; i++) {
    mats.push_back(testmat());
  }

  Material::Ptr big = SquashMat(mats);
  EXPECT_DOUBLE_EQ(testmat()->quantity()*n, big->quantity());

  double tot = 0;
  for (int i = 0; i < n; i++) {
    tot += mats[i]->quantity();
  }
  EXPECT_DOUBLE_EQ(testmat()->quantity()*n, tot);
}

TEST(ResManipTests, SquashProd) {
  std::vector<Product::Ptr> prods;
  int n = 3;
  for (int i = 0; i < n; i++) {
    prods.push_back(testprod());
  }

  Product::Ptr big = SquashProd(prods);
  EXPECT_DOUBLE_EQ(testprod()->quantity()*n, big->quantity());

  double tot = 0;
  for (int i = 0; i < n; i++) {
    tot += prods[i]->quantity();
  }
  EXPECT_DOUBLE_EQ(testprod()->quantity()*n, tot);
}

TEST(ResManipTests, Squash) {
  std::vector<Resource::Ptr> rs;
  int n = 3;
  for (int i = 0; i < n; i++) {
    rs.push_back(testmat());
  }

  // test Material-cast branch
  Resource::Ptr big = Squash(rs);
  EXPECT_DOUBLE_EQ(testmat()->quantity()*n, big->quantity());

  double tot = 0;
  for (int i = 0; i < n; i++) {
    tot += rs[i]->quantity();
  }
  EXPECT_DOUBLE_EQ(testmat()->quantity()*n, tot);

  // test Product-cast branch
  rs.clear();
  for (int i = 0; i < n; i++) {
    rs.push_back(testprod());
  }
  big = Squash(rs);
  EXPECT_DOUBLE_EQ(testprod()->quantity()*n, big->quantity());

  tot = 0;
  for (int i = 0; i < n; i++) {
    tot += rs[i]->quantity();
  }
  EXPECT_DOUBLE_EQ(testprod()->quantity()*n, tot);
}

TEST(ResManipTests, Separate) {
  Material::Ptr m = testmat();

  cyclus::toolkit::NucEffs effs;
  effs[922350000] = 1;
  effs[942390000] = 0.7;

  Material::Ptr stream = Separate(m, effs);
  cyclus::toolkit::MatQuery mq(m);
  cyclus::toolkit::MatQuery mqstream(stream);
  cyclus::toolkit::MatQuery mqref(testmat());


  // remains after separations are correct
  EXPECT_DOUBLE_EQ(0, mq.mass("U235")); // extracted all
  EXPECT_DOUBLE_EQ(mqref.mass("U238"), mq.mass("U238")); // unchanged
  EXPECT_DOUBLE_EQ(0.3*mqref.mass("Pu239"), mq.mass("Pu239")); // extracted part

  // nuclide mass conservation
  EXPECT_DOUBLE_EQ(mqref.mass("U235"), mqstream.mass("U235") + mq.mass("U235"));
  EXPECT_DOUBLE_EQ(mqref.mass("U238"), mqstream.mass("U238") + mq.mass("U238"));
  EXPECT_DOUBLE_EQ(mqref.mass("Pu239"), mqstream.mass("Pu239") + mq.mass("Pu239"));

}

