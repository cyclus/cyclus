// MaterialTests.cpp
#include <gtest/gtest.h>
#include "MaterialTests.h"
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
  EXPECT_TRUE(std::abs(same_as_orig_test_mat->quantity() - 
              test_mat_->quantity()) > EPS_KG);
  EXPECT_TRUE(same_as_orig_test_mat->checkQuality(test_mat_));
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, Extract) {
  mat_rsrc_ptr m1;
  ASSERT_NO_THROW(m1 = test_mat_->extract(test_comp_));
  EXPECT_FLOAT_EQ(test_mat_->quantity(), 0 );
  EXPECT_FLOAT_EQ(m1->quantity(), test_size_ );

  mat_rsrc_ptr m2;
  ASSERT_NO_THROW(m2 = diff_mat_->extract(test_comp_));
  EXPECT_FLOAT_EQ(diff_mat_->quantity(), test_size_*fraction );
  EXPECT_FLOAT_EQ(m2->quantity(), test_size_*(1-fraction) );

  // ASSERT_THROW(two_test_mat->extract(ten_test_comp), CycException);
}
