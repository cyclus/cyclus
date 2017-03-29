#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include "gis.h"

using namespace std;

using cyclus::GIS;

namespace cyclus {

class GISTest : public ::testing::Test {
 protected:
  GIS eiffel_, museum_, amsterdam_, barcelona_, urbana_, newyork_, bloomington_,
      sydney_, saopaulo_;
  virtual void SetUp() {
    eiffel_.set_latitude(48.858222);
    eiffel_.set_longitude(2.2945);
    museum_.set_latitude(48.861111);
    museum_.set_longitude(2.336389);
    amsterdam_.set_latitude(52.37305);
    amsterdam_.set_longitude(4.892222);
    barcelona_.set_latitude(41.383333);
    barcelona_.set_longitude(2.183333);
    urbana_.set_latitude(40.109665);
    urbana_.set_longitude(-88.204247);
    newyork_.set_latitude(40.7127);
    newyork_.set_longitude(-74.0059);
    bloomington_.set_latitude(40.484167);
    bloomington_.set_longitude(-88.993611);
    sydney_.set_latitude(-33.865);
    sydney_.set_longitude(151.209444);
    saopaulo_.set_latitude(-23.55);
    saopaulo_.set_longitude(-46.633333);
  }
  virtual void TearDown(){};
};

TEST_F(GISTest, Distance) {
  EXPECT_NEAR(eiffel_.Distance(museum_), 3.188, 3.188 * 0.05)
      << "eiffel vs museum failed";
  EXPECT_NEAR(eiffel_.Distance(amsterdam_), 432.126, 432.126 * 0.01)
      << "eiffel vs amsterdam failed";
  EXPECT_NEAR(amsterdam_.Distance(barcelona_), 1240.110, 1240.110 * 0.01)
      << "amsterdam vs barcelona failed";
  EXPECT_NEAR(amsterdam_.Distance(newyork_), 5868.701, 5868.701 * 0.01)
      << "amsterdam vs newyork failed";
  EXPECT_NEAR(newyork_.Distance(urbana_), 1204.246, 1204.246 * 0.01)
      << "newyork vs urbana failed";
  EXPECT_NEAR(bloomington_.Distance(urbana_), 78.663, 78.663 * 0.01)
      << "bloomington vs urbana failed";
}

TEST_F(GISTest, ToStringD) {
  string ams_str = amsterdam_.ToString(1);
  string syd_str = sydney_.ToString(1);
  string sao_str = saopaulo_.ToString(1);
  string urb_str = urbana_.ToString(1);

  ASSERT_TRUE(ams_str == "+52.37305+004.892222/");
  ASSERT_TRUE(syd_str == "-33.865+151.2094/");
  ASSERT_TRUE(sao_str == "-23.55-046.63334/");
  ASSERT_TRUE(urb_str == "+40.10966-088.20425/");
}

TEST_F(GISTest, ToStringDM) {
  string ams_str = amsterdam_.ToString(2);
  string syd_str = sydney_.ToString(2);
  string sao_str = saopaulo_.ToString(2);
  string urb_str = urbana_.ToString(2);
  ASSERT_TRUE(ams_str == "+5222.383+00453.533/");
  ASSERT_TRUE(syd_str == "-3351.9+15112.567/");
  ASSERT_TRUE(sao_str == "-2333-04638/");
  ASSERT_TRUE(urb_str == "+4006.5799-08812.255/");
}

TEST_F(GISTest, ToStringDMS) {
  string ams_str = amsterdam_.ToString(3);
  string syd_str = sydney_.ToString(3);
  string sao_str = saopaulo_.ToString(3);
  string urb_str = urbana_.ToString(3);
  ASSERT_TRUE(ams_str == "+522223.0+0045332.0/");
  ASSERT_TRUE(syd_str == "-335154.0+1511234.0/");
  ASSERT_TRUE(sao_str == "-233260.0-0463800.0/");
  ASSERT_TRUE(urb_str == "+400634.8-0881215.3/");
  ASSERT_LE(ams_str.length(), 20);
  ASSERT_LE(syd_str.length(), 20);
  ASSERT_LE(sao_str.length(), 20);
  ASSERT_LE(urb_str.length(), 20);
}
}
