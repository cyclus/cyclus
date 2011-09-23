#include <gtest/gtest.h>
#include "Material.h"
#include "Timer.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class MaterialTest : public ::testing::Test {
  protected:
    Iso u235, am241, th228, pb208;
    Atoms one;
    CompMap test_comp, decay_comp;
    string test_mat_unit;
    string test_rec_name;
    double test_size;
    Basis test_type;
    Material* test_mat;
    Material* decay_mat;
    long int u235_halflife;
    int th228_halflife;

    virtual void SetUp(){
      u235 = 92235;
      am241 = 95241;
      th228 = 90228;
      pb208 = 82208;
      Atoms one = 1.0;
      test_comp[u235]=one;
      decay_comp[u235]=one;
      decay_comp[th228]=one;
      test_mat_unit = "test_mat_unit";
      test_rec_name = "test_rec_name";
      test_size = 10.0;
      test_type = ATOMBASED;
      u235_halflife = 8445600000; // approximate, in months
      th228_halflife = 2*11; // approximate, in months
      int time = TI->getTime();

      test_mat = new Material(test_comp, test_mat_unit, test_rec_name, test_size, test_type); 
      decay_mat = new Material(decay_comp, test_mat_unit, test_rec_name, test_size, test_type); 
    };
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, ManualConstructor) {
  EXPECT_EQ(       test_mat->getUnits(),        "test_mat_unit"       );
  EXPECT_EQ(       test_mat->getTotAtoms(),     10                    );
  ASSERT_NEAR(     test_mat->getTotMass(),      2.35,            0.001); // 10mol U235 ~= 2.35kg
  ASSERT_NEAR(     test_mat->getMassComp(u235), 1,               0.001); // normalized 
  ASSERT_NEAR(     test_mat->getAtomComp(u235),     1,               0.001); // normalized 
  ASSERT_NEAR(     decay_mat->getAtomComp(am241), 0.0,               0.001); // americium is absent
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, ChangeComp) {
  test_mat->changeAtomComp(decay_comp,TI->getTime());
  ASSERT_NEAR(     test_mat->getTotMass(),      2.315,            0.001); // 10mol U235+Th228 ~= 2.315kg
  ASSERT_NEAR(     test_mat->getMassComp(u235), 0.508,            0.001); // normalized 
  ASSERT_NEAR(     test_mat->getAtomComp(u235),     0.5,              0.001); // normalized 
  ASSERT_NEAR(     test_mat->getAtomComp(am241),    0.0,              0.001); // americium is absent
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, Decay){
  Material::loadDecayInfo();
  test_mat->decay(2);
  ASSERT_NEAR(     test_mat->getAtomComp(u235),     1,              0.001);
  ASSERT_NEAR(     decay_mat->getAtomComp(u235),  0.5,              0.001);
  ASSERT_NEAR(     decay_mat->getAtomComp(th228), 0.5,              0.001);
  decay_mat->decay(th228_halflife);
  ASSERT_NEAR(     decay_mat->getAtomComp(th228),  0.25,            0.01);
  ASSERT_NEAR(     decay_mat->getAtomComp(pb208),  0.25,            0.01);
}
