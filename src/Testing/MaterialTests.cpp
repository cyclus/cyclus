#include <gtest/gtest.h>
#include "Material.h"


class MaterialTest : public ::testing::Test {
  protected:
    Iso u235;
    Atoms one;
    CompMap test_comp;
    string test_mat_unit;
    string test_rec_name;
    double test_size;
    Basis test_type;
    Material* test_mat;

    virtual void SetUp(){
      u235 = 92235;
      Atoms one = 1.0;
      test_comp[u235]=one;
      test_mat_unit = "test_mat_unit";
      test_rec_name = "test_rec_name";
      test_size = 10.0;
      test_type = atomBased;

      test_mat = new Material(test_comp, test_mat_unit, test_rec_name, test_size, test_type); 
    };
};

TEST_F(MaterialTest, ManualConstructor) {
  EXPECT_EQ(       test_mat->getUnits(),        "test_mat_unit"       );
  EXPECT_EQ(       test_mat->getTotAtoms(),     10                    );
  ASSERT_NEAR(     test_mat->getTotMass(),      2.35,            0.001); // 10mol U235 ~= 2.35kg
  ASSERT_NEAR(     test_mat->getMassComp(u235), 1,               0.001); // normalized 
  ASSERT_NEAR(     test_mat->getComp(u235),     1,               0.001); // normalized 
}



