#include <gtest/gtest.h>

#include "gis.h"
#include <string>
#include <iostream>

using namespace std;

using cyclus::GIS;

TEST(GISTests, check_distance){
  GIS eiffel(48.858222, 2.2945);
  GIS museum(48.861111, 2.336389);
  GIS amsterdam(52.373056, 4.892222);
  GIS barcelona(41.383333, 2.183333);
  GIS urbana(40.109665, -88.204247);
  GIS newyork(40.7127, -74.0059);
  GIS bloomington(40.484167, -88.993611);

  EXPECT_NEAR(eiffel.get_distance(museum), 3.188, 3.188*0.05);
  EXPECT_NEAR(eiffel.get_distance(amsterdam), 432.126, 432.126*0.01);
  EXPECT_NEAR(amsterdam.get_distance(barcelona), 1240.110, 1240.110*0.01);
  EXPECT_NEAR(amsterdam.get_distance(newyork), 6172.619, 6172.619*0.01);
  EXPECT_NEAR(newyork.get_distance(urbana), 1204.246, 1204.246*0.01);
  EXPECT_NEAR(bloomington.get_distance(urbana), 78.663, 78.663*0.01);
}

TEST(GISTests, check_toStringD){
  GIS amsterdam(52.373056, 4.892222);
  GIS sydney(-33.865, 151.209444);
  GIS saopaulo(-23.55, -46.633333);
  GIS urbana(40.109665, -88.204247);
  string ams_str = amsterdam.toStringD();
  string syd_str = sydney.toStringD();
  string sao_str = saopaulo.toStringD();
  string urb_str = urbana.toStringD();

  ASSERT_TRUE(ams_str == "+52.37305+004.892222/");
  ASSERT_TRUE(syd_str == "-33.865+151.2094/");
  ASSERT_TRUE(sao_str == "-23.55-046.63334/");
  ASSERT_TRUE(urb_str == "+40.10966-088.20425/");
}

TEST(GISTests, check_toStringDM){
  GIS amsterdam(52.373056, 4.892222);
  GIS sydney(-33.865, 151.209444);
  GIS saopaulo(-23.55, -46.633333);
  GIS urbana(40.109665, -88.204247);
  string ams_str = amsterdam.toStringDM();
  string syd_str = sydney.toStringDM();
  string sao_str = saopaulo.toStringDM();
  string urb_str = urbana.toStringDM();

  ASSERT_TRUE(ams_str == "+5222.383+00453.533/");
  ASSERT_TRUE(syd_str == "-3351.9+15112.567/");
  ASSERT_TRUE(sao_str == "-2333-04638/");
  ASSERT_TRUE(urb_str == "+4006.5799-08812.255/");
}

TEST(GISTests, check_toStringDMS){
  GIS amsterdam(52.373056, 4.892222);
  GIS sydney(-33.865, 151.209444);
  GIS saopaulo(-23.55, -46.633333);
  GIS urbana(40.109665, -88.204247);
  string ams_str = amsterdam.toStringDMS();
  string syd_str = sydney.toStringDMS();
  string sao_str = saopaulo.toStringDMS();
  string urb_str = urbana.toStringDMS();

  ASSERT_TRUE(ams_str == "+522223.0+0045332.0/");
  ASSERT_TRUE(syd_str == "-335154.0+1511234.0/");
  ASSERT_TRUE(sao_str == "-233260.0-0463800.0/");
  ASSERT_TRUE(urb_str == "+400634.8-0881215.3/");
  ASSERT_LE(ams_str.length(), 20);
  ASSERT_LE(syd_str.length(), 20);
  ASSERT_LE(sao_str.length(), 20);
  ASSERT_LE(urb_str.length(), 20);
}