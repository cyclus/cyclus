#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include "gis.h"

using namespace std;

using cyclus::GIS;

class GISTest : public ::testing::TEST {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
  GIS eiffel(48.858222, 2.2945);
  GIS museum(48.861111, 2.336389);
  GIS amsterdam(52.373056, 4.892222);
  GIS barcelona(41.383333, 2.183333);
  GIS urbana(40.109665, -88.204247);
  GIS newyork(40.7127, -74.0059);
  GIS bloomington(40.484167, -88.993611);
  GIS amsterdam(52.373056, 4.892222);
  GIS sydney(-33.865, 151.209444);
  GIS saopaulo(-23.55, -46.633333);
}

TEST(GISTests, Distance) {
  EXPECT_NEAR(eiffel.Distance(museum), 3.188, 3.188 * 0.05)
      << "eiffel vs museum failed";
  EXPECT_NEAR(eiffel.Distance(amsterdam), 432.126, 432.126 * 0.01)
      << "eiffel vs amsterdam failed";
  EXPECT_NEAR(amsterdam.Distance(barcelona), 1240.110, 1240.110 * 0.01)
      << "amsterdam vs barcelona failed";
  EXPECT_NEAR(amsterdam.Distance(newyork), 5868.701, 5868.701 * 0.01)
      << "amsterdam vs newyork failed";
  EXPECT_NEAR(newyork.Distance(urbana), 1204.246, 1204.246 * 0.01)
      << "newyork vs urbana failed";
  EXPECT_NEAR(bloomington.Distance(urbana), 78.663, 78.663 * 0.01)
      << "bloomington vs urbana failed";
}

TEST(GISTests, ToStringD) {
  string ams_str = amsterdam.toString("d");
  string syd_str = sydney.toString("d");
  string sao_str = saopaulo.toString("d");
  string urb_str = urbana.toString("d");

  ASSERT_TRUE(ams_str == "+52.37305+004.892222/");
  ASSERT_TRUE(syd_str == "-33.865+151.2094/");
  ASSERT_TRUE(sao_str == "-23.55-046.63334/");
  ASSERT_TRUE(urb_str == "+40.10966-088.20425/");
}

TEST(GISTests, ToStringDM) {
  string ams_str = amsterdam.toString("dm");
  string syd_str = sydney.toString("dm");
  string sao_str = saopaulo.toString("dm");
  string urb_str = urbana.toString("dm");

  ASSERT_TRUE(ams_str == "+5222.383+00453.533/");
  ASSERT_TRUE(syd_str == "-3351.9+15112.567/");
  ASSERT_TRUE(sao_str == "-2333-04638/");
  ASSERT_TRUE(urb_str == "+4006.5799-08812.255/");
}

TEST(GISTests, ToStringDMS) {
  string ams_str = amsterdam.toString("dms");
  string syd_str = sydney.toString("dms");
  string sao_str = saopaulo.toString("dms");
  string urb_str = urbana.toString("dms");

  ASSERT_TRUE(ams_str == "+522223.0+0045332.0/");
  ASSERT_TRUE(syd_str == "-335154.0+1511234.0/");
  ASSERT_TRUE(sao_str == "-233260.0-0463800.0/");
  ASSERT_TRUE(urb_str == "+400634.8-0881215.3/");
  ASSERT_LE(ams_str.length(), 20);
  ASSERT_LE(syd_str.length(), 20);
  ASSERT_LE(sao_str.length(), 20);
  ASSERT_LE(urb_str.length(), 20);
}