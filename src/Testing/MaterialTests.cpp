#include <gtest/gtest.h>
#include "Material.h"

TEST(MaterialTest, ManualConstructor){
  CompMap test_comp;
  string test_mat_unit;
  string test_rec_name;
  double test_size;
  Basis test_type;
  Material* test_mat;

  Iso u235 = 92235;
  Atoms one = 1.0;
  test_comp[u235]=one;
  test_mat_unit = "test_mat_unit";
  test_rec_name = "test_rec_name";
  test_size = 10.0;
  test_type = atomBased;

  test_mat = new Material(test_comp, test_mat_unit, test_rec_name, test_size, test_type); 
  //Material* test_mat = new Material(); 

  EXPECT_EQ(test_mat->getUnits(), "test_mat_unit");
  //EXPECT_EQ(MaterialTest::test_mat->getTotAtoms(),0);
} // MaterialTest



