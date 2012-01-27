// MaterialTests.cpp
#include <gtest/gtest.h>
#include "Material.h"
#include "IsoVector.h"
#include "Timer.h"
#include "CycException.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class MaterialTest : public ::testing::Test {
  protected:
    Iso u235_, am241_, th228_, pb208_;
    int one_mol_; // atoms
    CompMap test_comp_, diff_comp;
    double test_size_;
    Material* test_mat_;
    Material* diff_mat_;
    long int u235_halflife_;
    int th228_halflife_;

    virtual void SetUp(){
      u235_ = 92235;
      am241_ = 95241;
      th228_ = 90228;
      pb208_ = 82208;
      one_mol_ = 1.0;
      test_comp_[u235_]=one_mol_;
      diff_comp[pb208_]=one_mol_;
      diff_comp[am241_]=one_mol_;
      test_size_ = 10.0;
      u235_halflife_ = 8445600000; // approximate, in months
      th228_halflife_ = 2*11; // approximate, in months
      int time_ = TI->time();

      test_mat_ = new Material(test_comp_);
      diff_mat_ = new Material(diff_comp);
    }

    virtual void TearDown(){
      delete test_mat_;
      delete diff_mat_;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, Constructors){
  EXPECT_EQ(test_mat_->units(), "kg");
  EXPECT_EQ(test_mat_->type(), MATERIAL_RES);
  EXPECT_GE(test_mat_->ID(),0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, Clone) {
  Material* clone_mat;
  ASSERT_NO_THROW(clone_mat = test_mat_->clone());

  // in order to acommodate discrete material tracking, all ID's mush be unique
  EXPECT_NE(test_mat_->ID(), clone_mat->ID());

  EXPECT_EQ(test_mat_->quantity(), clone_mat->quantity());
  EXPECT_EQ(test_mat_->type(), clone_mat->type());
  EXPECT_TRUE(test_mat_->checkQuality(clone_mat));
  EXPECT_TRUE(test_mat_->checkQuantityEqual(clone_mat));
  EXPECT_TRUE(clone_mat->checkQuality(test_mat_));
  EXPECT_TRUE(clone_mat->checkQuantityEqual(test_mat_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, AbsorbLikeMaterial) {
  // make a number of materials masses 1, 2, and 10 
  CompMap two_test_comp, ten_test_comp, pu_test_comp;
  Material* one_test_mat;
  Material* two_test_mat;
  Material* ten_test_mat;
  one_test_mat = new Material(test_comp_);

  CompMap::iterator it;
  for(it = test_comp_.begin(); it != test_comp_.end(); it++){
    two_test_comp[it->first] = 2 * (it->second);
    ten_test_comp[it->first] = 10 * (it->second);
  }
  two_test_mat = new Material(two_test_comp);
  ten_test_mat = new Material(ten_test_comp);

  // see that two materials with the same composition do the right thing
  double orig = test_mat_->quantity();
  ASSERT_NO_THROW(test_mat_->absorb(one_test_mat));
  EXPECT_FLOAT_EQ(test_mat_->quantity(), 2 * orig );

  ASSERT_NO_THROW(test_mat_->absorb(two_test_mat));
  EXPECT_FLOAT_EQ(test_mat_->quantity(), 4 * orig );

  ASSERT_NO_THROW(test_mat_->absorb(ten_test_mat));
  EXPECT_FLOAT_EQ(test_mat_->quantity(), 14 * orig );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, AbsorbUnLikeMaterial) {
  // make a number of materials masses 1, 2, and 10 
  Material* same_as_orig_test_mat = new Material(test_comp_);

  CompMap diff_test_comp;
  diff_test_comp[pb208_] = 1.0;
  diff_test_comp[am241_] = 1.0;
  diff_test_comp[th228_] = 1.0;
  Material* diff_test_mat = new Material(diff_test_comp);

  double orig = test_mat_->quantity();
  double origdiff = diff_test_mat->quantity();

  // see that materials with different compositions do the right thing
  ASSERT_NO_THROW(test_mat_->absorb(diff_test_mat));
  EXPECT_FLOAT_EQ(orig + origdiff, test_mat_->quantity() );
  EXPECT_FALSE(same_as_orig_test_mat->checkQuantityEqual(test_mat_));
  EXPECT_FALSE(same_as_orig_test_mat->checkQuality(test_mat_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, ExtractLikeVector) {
  // make a number of materials masses 1, 2, and 10 
  CompMap two_test_comp, ten_test_comp, pu_test_comp;
  Material* one_test_mat;
  Material* two_test_mat;
  Material* ten_test_mat;
  one_test_mat = new Material(test_comp_);

  CompMap::iterator it;
  for(it=test_comp_.begin(); it!=test_comp_.end(); it++){
    two_test_comp[it->first]=2*(it->second);
    ten_test_comp[it->first]=10*(it->second);
  }
  two_test_mat = new Material(two_test_comp);
  ten_test_mat = new Material(ten_test_comp);

  // see that two materials with the same composition do the right thing
  double orig = test_mat_->quantity();

  ASSERT_NO_THROW(test_mat_->extract(test_comp_));
  EXPECT_FLOAT_EQ(test_mat_->quantity(), 0 );

  ASSERT_NO_THROW(ten_test_mat->extract(two_test_comp));
  EXPECT_FLOAT_EQ(ten_test_mat->quantity(),8*orig );

  ASSERT_THROW(two_test_mat->extract(ten_test_comp), CycException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, ExtractUnLikeVector) {
  Material* same_as_orig_test_mat = new Material(test_comp_);
  CompMap half_test_comp, quarter_test_comp;
  half_test_comp[u235_]=.5;
  quarter_test_comp[u235_]=.25;

  double orig = test_mat_->quantity();

  // see that materials with different compositions do the right thing
  ASSERT_NO_THROW(test_mat_->extract(half_test_comp));
  EXPECT_FLOAT_EQ(0.5*orig, test_mat_->quantity() );
  EXPECT_FALSE(same_as_orig_test_mat->checkQuantityEqual(test_mat_));
  EXPECT_FALSE(same_as_orig_test_mat->checkQuality(test_mat_));
  EXPECT_GT(same_as_orig_test_mat->quantity(), test_mat_->quantity());
  
  ASSERT_NO_THROW(test_mat_->extract(quarter_test_comp));
  EXPECT_FLOAT_EQ(0.25*orig, test_mat_->quantity() );
  EXPECT_FALSE(same_as_orig_test_mat->checkQuantityEqual(test_mat_));
  EXPECT_FALSE(same_as_orig_test_mat->checkQuality(test_mat_));
  EXPECT_GT(same_as_orig_test_mat->quantity(), test_mat_->quantity());

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, ExtractMass) {
  // make a number of materials masses 1, 2, and 10 
  double mass_to_rem = 0.25*(diff_mat_->quantity());
  double mass_remaining = 0.75*(diff_mat_->quantity());
  IsoVector* orig = new IsoVector(diff_comp);
  double am_orig = orig->mass(am241_);
  double pb_orig= orig->mass(pb208_);

  // see that two materials with the same composition do the right thing
  ASSERT_NO_THROW(diff_mat_->extract(mass_to_rem));
  EXPECT_FLOAT_EQ(diff_mat_->quantity(), mass_remaining );

  IsoVector remaining = diff_mat_->isoVector();
  double am_remaining = remaining.mass(am241_);
  double pb_remaining = remaining.mass(pb208_);

  EXPECT_FLOAT_EQ(am_remaining/pb_remaining, am_orig/pb_orig);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, GetSetQuantity) {

}
