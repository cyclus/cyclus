
#include <gtest/gtest.h>

#include "cyclus.h"

using cyclus::Material;
using cyclus::Product;
using cyclus::Resource;
using cyclus::toolkit::Squash;


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

TEST(ResManipTests, Squash_Mat) {
  std::vector<Material::Ptr> mats;
  int n = 3;
  for (int i = 0; i < n; i++) {
    mats.push_back(testmat());
  }

  Material::Ptr big = Squash(mats);
  EXPECT_DOUBLE_EQ(testmat()->quantity()*n, big->quantity());

  double tot = 0;
  for (int i = 0; i < n; i++) {
    tot += mats[i]->quantity();
  }
  EXPECT_DOUBLE_EQ(testmat()->quantity()*n, tot);
}

TEST(ResManipTests, Squash_Prod) {
  std::vector<Product::Ptr> prods;
  int n = 3;
  for (int i = 0; i < n; i++) {
    prods.push_back(testprod());
  }

  Product::Ptr big = Squash(prods);
  EXPECT_DOUBLE_EQ(testprod()->quantity()*n, big->quantity());

  double tot = 0;
  for (int i = 0; i < n; i++) {
    tot += prods[i]->quantity();
  }
  EXPECT_DOUBLE_EQ(testprod()->quantity()*n, tot);
}

TEST(ResManipTests, Squash_Res) {
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

