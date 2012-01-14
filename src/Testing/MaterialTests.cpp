// MaterialTests.cpp
#include <gtest/gtest.h>
#include "Material.h"
#include "IsoVector.h"
#include "Timer.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class MaterialTest : public ::testing::Test {
  protected:
    Iso u235_, am241_, th228_, pb208_;
    int one_mol_; // atoms
    CompMap test_comp_, decay_comp;
    double test_size_;
    Material* test_mat_;
    Material* decay_mat_;
    long int u235_halflife_;
    int th228_halflife_;

    virtual void SetUp(){
      test_mat_ = new Material(test_comp_);
      u235_ = 92235;
      am241_ = 95241;
      th228_ = 90228;
      pb208_ = 82208;
      one_mol_ = 1.0;
      test_comp_[u235_]=one_mol_;
      decay_comp[u235_]=one_mol_;
      decay_comp[th228_]=one_mol_;
      test_size_ = 10.0;
      u235_halflife_ = 8445600000; // approximate, in months
      th228_halflife_ = 2*11; // approximate, in months
      int time_ = TI->getTime();

      test_mat_ = new Material(test_comp_);
      decay_mat_ = new Material(decay_comp);
    }

    virtual void TearDown(){
      delete test_mat_;
      delete decay_mat_;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, Constructors){
  EXPECT_EQ(test_mat_->getResourceUnits(), "kg");
  EXPECT_EQ(test_mat_->getResourceType(), MATERIAL_RES);
  EXPECT_GE(test_mat_->ID(),0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, Clone) {
  Material* clone_mat;
  EXPECT_NO_THROW(clone_mat = test_mat_->clone());
  ASSERT_EQ(test_mat_->ID(),clone_mat->ID()); // we want the IDs to be the same, yes?
  ASSERT_EQ(test_mat_->getQuantity(),clone_mat->getQuantity());
  ASSERT_EQ(test_mat_->getResourceType(),clone_mat->getResourceType());
  ASSERT_TRUE(test_mat_->checkQuality(clone_mat));
  ASSERT_TRUE(test_mat_->checkQuantityEqual(clone_mat));
  ASSERT_TRUE(clone_mat->checkQuality(test_mat_));
  ASSERT_TRUE(clone_mat->checkQuantityEqual(test_mat_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, DISABLED_Absorb) {

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, DISABLED_ManualConstructor) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, DISABLED_ChangeComp) {
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, DISABLED_Decay){
}
