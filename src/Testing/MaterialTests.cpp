// MaterialTests.cpp
#include <gtest/gtest.h>
#include "MaterialTests.h"
#include "CycLimits.h"
#include <cmath>

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
  EXPECT_DOUBLE_EQ(test_mat_->quantity(), clone_mat->quantity());
  EXPECT_TRUE(clone_mat->checkQuality(test_mat_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, setQuantity) {
  EXPECT_FLOAT_EQ( test_size_, test_mat_->quantity());
  double new_size = test_size_/2;
  ASSERT_NO_THROW( test_mat_->setQuantity(new_size));
  EXPECT_FLOAT_EQ( new_size , test_mat_->quantity());

  new_size = new_size*10;
  ASSERT_NO_THROW( test_mat_->setQuantity(new_size, KG));
  EXPECT_FLOAT_EQ( new_size , test_mat_->quantity());

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, CheckQuality) {
  rsrc_ptr test(test_mat_);
  rsrc_ptr diff(diff_mat_);
  rsrc_ptr gen(new GenericResource("kg", "foo", 10));

  EXPECT_TRUE(test->checkQuality(diff));
  EXPECT_TRUE(diff->checkQuality(test));
  EXPECT_FALSE(test->checkQuality(gen));
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, CheckMass) {
  // check total mass, you'll use it later.
  EXPECT_FLOAT_EQ(test_mat_->quantity(),test_size_); 
  ASSERT_FLOAT_EQ(1000.0*(test_mat_->quantity()), test_mat_->mass(G));

  ASSERT_FLOAT_EQ(test_size_, test_mat_->mass(KG));
  EXPECT_NO_THROW(test_mat_->setQuantity(test_size_*1000.0,G));
  ASSERT_FLOAT_EQ(test_size_, test_mat_->mass(KG));
  // reset the size to be 1000 times less, by changing units to 
  EXPECT_NO_THROW(test_mat_->setQuantity(test_size_,G));
  ASSERT_FLOAT_EQ(test_size_/1000.0, test_mat_->mass(KG));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, CheckIsoMass) {
  // check total mass, you'll use it later.
  EXPECT_FLOAT_EQ(test_mat_->quantity(),test_size_); 

  // what's the mass per atom ratio?
  ASSERT_NEAR(u235_g_per_mol_, (*test_mat_->isoVector().comp()).mass_to_atom_ratio(), 0.1);
  // you should be able to get the mass per isotope
  EXPECT_NO_THROW(test_mat_->mass(u235_));

  // if the material has only one isotope, it should be the same as the total
  // the u235 mass should be (mol/mat)(1kg/1000g)(235g/mol)  
  ASSERT_FLOAT_EQ(test_mat_->mass(u235_), test_mat_->moles(u235_)*u235_g_per_mol_/1000.0); 
  // you should be able to get the mass per isotope
  EXPECT_NO_THROW(test_mat_->mass(u235_));
  ASSERT_FLOAT_EQ(test_mat_->mass(u235_), test_mat_->mass(u235_,KG));
  ASSERT_FLOAT_EQ(test_mat_->mass(am241_), 0);

  // if the mat has many isotopes, their individual masses should scale with 
  // their atomic numbers.
  double test_total = 0;
  CompMap::iterator comp; 
  int i;
  for( comp = (*test_comp_).begin(); comp != (*test_comp_).end(); ++comp){
    i = (*comp).first;
    test_total += test_mat_->mass(i);
  }
  ASSERT_FLOAT_EQ(test_mat_->quantity(), test_total);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, CheckIsoAtoms){

  // you should be able to get to the atoms of a certain iso in your material
  EXPECT_NO_THROW(test_mat_->moles(u235_));
  EXPECT_EQ(true, test_comp_->normalized());
  ASSERT_NEAR(u235_g_per_mol_, (*test_mat_->isoVector().comp()).mass_to_atom_ratio(), 0.1);
  EXPECT_FLOAT_EQ(1000*test_size_/u235_g_per_mol_, test_mat_->moles(u235_));
  ASSERT_FLOAT_EQ(test_mat_->moles(am241_), 0);

  // a mat's total atoms should be the total of all the contained isotopes. 
  double total_atoms = 0;
  CompMap::iterator comp; 
  int i;
  for( comp = (*test_comp_).begin(); comp != (*test_comp_).end(); ++comp){
    i = (*comp).first;
    total_atoms += test_mat_->moles(i);
  }
  // you should be able to get to the total atoms in your material
  ASSERT_FLOAT_EQ(test_mat_->moles(), total_atoms);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, CheckConvertFromKg){
  EXPECT_FLOAT_EQ(1000, test_mat_->convertFromKg(1,G)); // 1000g = 1 kg
  EXPECT_FLOAT_EQ(1000, test_mat_->convertFromKg(1000,KG)); // 1000 kg = 1000 kg
  EXPECT_FLOAT_EQ(1, test_mat_->convertFromKg(1,KG)); // 1 kg = 1 kg
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, CheckConvertToKg){
  EXPECT_FLOAT_EQ(1, test_mat_->convertToKg(1000,G)); // 1kg = 1000 g
  EXPECT_FLOAT_EQ(1000, test_mat_->convertToKg(1000,KG)); // 1000kg = 1000kg
  EXPECT_FLOAT_EQ(1, test_mat_->convertToKg(1,KG)); // 1kg = 1kg
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, SimpleAbsorb) {
  double val = 1.5;
  mat_rsrc_ptr m1 = mat_rsrc_ptr(new Material(test_comp_));
  mat_rsrc_ptr m2 = mat_rsrc_ptr(new Material(test_comp_));
  m1->setQuantity(val);
  m2->setQuantity(val);
  ASSERT_EQ(m1->isoVector(),m2->isoVector());
  ASSERT_EQ(m1->quantity(),m2->quantity());

  m2->absorb(m1);

  ASSERT_EQ(m1->isoVector(),m2->isoVector());
  ASSERT_EQ(2*m1->quantity(),m2->quantity());
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
  ASSERT_EQ(test_mat_->isoVector(),one_test_mat->isoVector());
  EXPECT_FLOAT_EQ(test_mat_->quantity(), factor * orig );

  factor += two;
  ASSERT_NO_THROW(test_mat_->absorb(two_test_mat));
  ASSERT_EQ(test_mat_->isoVector(),two_test_mat->isoVector());
  EXPECT_FLOAT_EQ(test_mat_->quantity(), factor * orig );

  factor += ten;
  ASSERT_NO_THROW(test_mat_->absorb(ten_test_mat));
  ASSERT_EQ(test_mat_->isoVector(),ten_test_mat->isoVector());
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
  EXPECT_TRUE(std::abs(same_as_orig_test_mat->quantity() - 
              test_mat_->quantity()) > cyclus::eps_rsrc());
  EXPECT_TRUE(same_as_orig_test_mat->checkQuality(test_mat_));

  // see that an empty material appropriately absorbs a not empty material.
  ASSERT_NO_THROW(default_mat_->absorb(test_mat_));
  EXPECT_FLOAT_EQ(orig + origdiff, default_mat_->quantity() );
  EXPECT_FLOAT_EQ(orig + origdiff, default_mat_->mass(KG) );

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, ExtractMass) {
  double amt = test_size_ / 3;
  double diff = test_size_ - amt;  
  mat_rsrc_ptr extracted;
  EXPECT_FLOAT_EQ(test_mat_->quantity(),test_size_); // we expect this amt
  EXPECT_NO_THROW(extracted = test_mat_->extract(amt)); // extract an amt
  EXPECT_FLOAT_EQ(extracted->quantity(),amt); // check correctness
  EXPECT_FLOAT_EQ(test_mat_->quantity(),diff); // check correctness
  EXPECT_EQ(test_mat_->isoVector(),extracted->isoVector());
  EXPECT_THROW(two_test_mat_->extract(2*two_test_mat_->quantity()), CycException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, Extract_complete) {

  // Complete extraction
  mat_rsrc_ptr m1;
  EXPECT_NO_THROW( m1 = test_mat_->extract(test_comp_, test_size_));
  EXPECT_TRUE( m1->isoVector().compEquals(test_comp_));
  EXPECT_FLOAT_EQ( 0, test_mat_->quantity() );
  EXPECT_FLOAT_EQ( test_size_, m1->quantity() );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, Extract_over_extract) {

  // Over-extraction should throw an exception
  EXPECT_THROW( diff_mat_->extract(test_comp_, 2*test_size_), CycNegativeValueException);
  EXPECT_THROW( test_mat_->extract(test_comp_, 2*test_size_), CycException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, Extract_half) {
  // two minus one equals one.
  mat_rsrc_ptr m1;
  m1 = two_test_mat_->extract(test_comp_, test_size_);
  EXPECT_FLOAT_EQ( test_size_, m1->quantity() );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, Extract_diff_comp) {

  // differing comp minus one element equals old comp minus new
  mat_rsrc_ptr m1;
  double orig = diff_mat_->quantity();
  double orig_u235 = diff_mat_->moles(u235_);
  double orig_am241 = diff_mat_->moles(am241_);
  double orig_pb208 = diff_mat_->moles(pb208_);
  EXPECT_NO_THROW( m1 = diff_mat_->extract( test_comp_, 1 )); 
  EXPECT_FLOAT_EQ( orig - m1->quantity(), diff_mat_->quantity() );
  EXPECT_TRUE( m1->isoVector().compEquals(test_comp_));
  EXPECT_FALSE( diff_mat_->isoVector().compEquals(test_comp_));
  EXPECT_FLOAT_EQ( orig_am241, diff_mat_->moles(am241_) );
  EXPECT_FLOAT_EQ( orig_pb208, diff_mat_->moles(pb208_) );
  EXPECT_NE( orig_u235, diff_mat_->moles(u235_) );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, Absorb_then_extract) {

  CompMapPtr comp_to_rem = CompMapPtr(test_comp_);
  double kg_to_rem = 0.25*test_size_; 

  // if you start with an empty material
  EXPECT_FLOAT_EQ(0, default_mat_->quantity());
  // then you absorb another material, they should be identical
  EXPECT_NO_THROW(default_mat_->absorb(test_mat_));
  EXPECT_TRUE(default_mat_->checkQuality(test_mat_));
  EXPECT_EQ(default_mat_->isoVector(),test_mat_->isoVector());
  EXPECT_FLOAT_EQ(test_size_, default_mat_->quantity());
  // and it should be okay to extract a fraction of the original composiiton 
  EXPECT_NO_THROW(default_mat_->extract(comp_to_rem, kg_to_rem));
  EXPECT_FLOAT_EQ(test_size_-kg_to_rem, default_mat_->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, Extract_in_grams) {

  CompMapPtr comp_to_rem = CompMapPtr(test_comp_);
  double kg_to_rem = 0.25*test_size_; 
  double g_to_rem = 1000*kg_to_rem;

  // if you start with an empty material
  EXPECT_FLOAT_EQ(0, default_mat_->quantity());
  // then you absorb another material, they should be identical
  EXPECT_NO_THROW(default_mat_->absorb(test_mat_));
  EXPECT_EQ(default_mat_->isoVector(),test_mat_->isoVector());
  EXPECT_FLOAT_EQ(test_size_, default_mat_->quantity());
  // and it should be okay to extract part of the original composiiton IN GRAMS 
  EXPECT_NO_THROW(default_mat_->extract(comp_to_rem, g_to_rem, G));
  EXPECT_FLOAT_EQ(test_size_-kg_to_rem, default_mat_->quantity());
}



