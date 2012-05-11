// MaterialTests.cpp
#include <gtest/gtest.h>
#include "MaterialTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, Constructors){
  EXPECT_EQ(test_mat_->units(), "kg");
  EXPECT_EQ(test_mat_->type(), MATERIAL_RES);
  EXPECT_GE(test_mat_->ID(),0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, Clone) {
  rsrc_ptr clone_mat;
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
  mat_rsrc_ptr one_test_mat;
  mat_rsrc_ptr two_test_mat;
  mat_rsrc_ptr ten_test_mat;
  one_test_mat = mat_rsrc_ptr(new Material(test_comp_));
  int one = 1;
  one_test_mat->setQuantity(one*test_size_);
  two_test_mat = mat_rsrc_ptr(new Material(test_comp_));
  int two = 2;
  two_test_mat->setQuantity(two*test_size_);
  ten_test_mat = mat_rsrc_ptr(new Material(test_comp_));
  int ten = 10;
  ten_test_mat->setQuantity(ten*test_size_);

  // see that two materials with the same composition do the right thing
  double orig = test_mat_->quantity();
  int factor = 1+one;
  ASSERT_NO_THROW(test_mat_->absorb(one_test_mat));
  EXPECT_FLOAT_EQ(test_mat_->quantity(), factor * orig );

  factor += two;
  ASSERT_NO_THROW(test_mat_->absorb(two_test_mat));
  EXPECT_FLOAT_EQ(test_mat_->quantity(), factor * orig );

  factor += ten;
  ASSERT_NO_THROW(test_mat_->absorb(ten_test_mat));
  EXPECT_FLOAT_EQ(test_mat_->quantity(), factor * orig );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, AbsorbUnLikeMaterial) {
  // make a number of materials masses 1, 2, and 10 
  mat_rsrc_ptr same_as_orig_test_mat = mat_rsrc_ptr(new Material(test_comp_));

  CompMapPtr diff_test_comp = CompMapPtr(new CompMap(MASS));
  (*diff_test_comp)[pb208_] = 1.0;
  (*diff_test_comp)[am241_] = 1.0;
  (*diff_test_comp)[th228_] = 1.0;
  mat_rsrc_ptr diff_test_mat = mat_rsrc_ptr(new Material(diff_test_comp));
  diff_test_mat->setQuantity(test_size_/2);

  double orig = test_mat_->quantity();
  double origdiff = diff_test_mat->quantity();

  // see that materials with different compositions do the right thing
  ASSERT_NO_THROW(test_mat_->absorb(diff_test_mat));
  EXPECT_FLOAT_EQ(orig + origdiff, test_mat_->quantity() );
  EXPECT_FALSE(same_as_orig_test_mat->checkQuantityEqual(test_mat_));
  EXPECT_TRUE(same_as_orig_test_mat->checkQuality(test_mat_));
}

// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
// TEST_F(MaterialTest, ExtractLikeVector) {
//   // make a number of materials masses 1, 2, and 10 
//   CompMap two_test_comp, ten_test_comp, pu_test_comp;
//   mat_rsrc_ptr one_test_mat;
//   mat_rsrc_ptr two_test_mat;
//   mat_rsrc_ptr ten_test_mat;
//   one_test_mat = mat_rsrc_ptr(new Material(test_comp_));

//   CompMap::iterator it;
//   for(it=test_comp_->begin(); it!=test_comp_->end(); it++){
//     two_test_comp[it->first]=2*(it->second);
//     ten_test_comp[it->first]=10*(it->second);
//   }
//   two_test_mat = mat_rsrc_ptr(new Material(two_test_comp));
//   ten_test_mat = mat_rsrc_ptr(new Material(ten_test_comp));

//   // see that two materials with the same composition do the right thing
//   double orig = test_mat_->quantity();

//   ASSERT_NO_THROW(test_mat_->extract(test_comp_));
//   EXPECT_FLOAT_EQ(test_mat_->quantity(), 0 );

//   ASSERT_NO_THROW(ten_test_mat->extract(two_test_comp));
//   EXPECT_FLOAT_EQ(ten_test_mat->quantity(),8*orig );

//   ASSERT_THROW(two_test_mat->extract(ten_test_comp), CycException);
// }

// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
// TEST_F(MaterialTest, ExtractUnLikeVector) {
//   mat_rsrc_ptr same_as_orig_test_mat = mat_rsrc_ptr(new Material(test_comp_));
//   CompMap half_test_comp, quarter_test_comp;
//   half_test_comp[u235_]=.5;
//   quarter_test_comp[u235_]=.25;

//   double orig = test_mat_->quantity();

//   // see that materials with different compositions do the right thing
//   ASSERT_NO_THROW(test_mat_->extract(half_test_comp));
//   EXPECT_FLOAT_EQ(0.5*orig, test_mat_->quantity() );
//   EXPECT_FALSE(same_as_orig_test_mat->checkQuantityEqual(test_mat_));
//   EXPECT_TRUE(same_as_orig_test_mat->checkQuality(test_mat_));
//   EXPECT_GT(same_as_orig_test_mat->quantity(), test_mat_->quantity());
  
//   ASSERT_NO_THROW(test_mat_->extract(quarter_test_comp));
//   EXPECT_FLOAT_EQ(0.25*orig, test_mat_->quantity() );
//   EXPECT_FALSE(same_as_orig_test_mat->checkQuantityEqual(test_mat_));
//   EXPECT_TRUE(same_as_orig_test_mat->checkQuality(test_mat_));
//   EXPECT_GT(same_as_orig_test_mat->quantity(), test_mat_->quantity());

// }

// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
// TEST_F(MaterialTest, ExtractMass) {
//   // make a number of materials masses 1, 2, and 10 
//   double mass_to_rem = 0.25*(diff_mat_->quantity());
//   double mass_remaining = 0.75*(diff_mat_->quantity());
//   IsoVector* orig = new IsoVector(diff_comp);
//   double am_orig = orig->mass(am241_);
//   double pb_orig= orig->mass(pb208_);

//   // see that two materials with the same composition do the right thing
//   ASSERT_NO_THROW(diff_mat_->extract(mass_to_rem));
//   EXPECT_FLOAT_EQ(diff_mat_->quantity(), mass_remaining );

//   IsoVector remaining = diff_mat_->isoVector();
//   double am_remaining = remaining.mass(am241_);
//   double pb_remaining = remaining.mass(pb208_);

//   EXPECT_FLOAT_EQ(am_remaining/pb_remaining, am_orig/pb_orig);
// }

// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
// TEST_F(MaterialTest, GetSetQuantity) {

// }

// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
// TEST_F(MaterialTest, CheckQuality) {
//   rsrc_ptr test(test_mat_);
//   rsrc_ptr diff(diff_mat_);
//   rsrc_ptr gen(new GenericResource("kg", "foo", 10));

//   EXPECT_TRUE(test->checkQuality(diff));
//   EXPECT_TRUE(diff->checkQuality(test));
//   EXPECT_FALSE(test->checkQuality(gen));
// }
