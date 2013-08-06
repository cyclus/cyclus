// material_tests.cc
#include <gtest/gtest.h>
#include "material.h"
#include "generic_resource.h"
#include "iso_vector.h"
#include "timer.h"
#include "error.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class MaterialTest : public ::testing::Test {
  protected:
    cyclus::Iso u235_, am241_, th228_, pb208_, pu239_;
    int one_g_; // grams
    double one_; // unitless
    double half_; // unitless
    cyclus::CompMapPtr test_comp_, two_test_comp_, non_norm_test_comp_, diff_comp_, 
               pu_comp_, u_am_comp_;
    double test_size_, fraction;
    cyclus::Material::Ptr test_mat_, two_test_mat_, ten_test_mat_, pu_mat_, u_am_mat_;
    cyclus::Material::Ptr diff_mat_;
    cyclus::Material::Ptr default_mat_;
    long int u235_halflife_;
    int th228_halflife_;
    double u235_g_per_mol_;
    std::map<cyclus::Iso, double> test_vec_;

    virtual void SetUp(){
      // composition set up
      u235_ = 92235;
      am241_ = 95241;
      th228_ = 90228;
      pb208_ = 82208;
      pu239_ = 94239;
      one_g_ = 1.0;
      one_ = 1.0;
      half_ = 0.5;
      test_size_ = 10.0;
      fraction = 2.0 / 3.0;

      // composition creation
      test_comp_ = cyclus::CompMapPtr(new cyclus::CompMap(cyclus::MASS));
      (*test_comp_)[u235_]=one_g_;
      (*test_comp_).normalize();

      two_test_comp_ = cyclus::CompMapPtr(new cyclus::CompMap(cyclus::MASS));
      (*two_test_comp_)[u235_]=one_g_;

      non_norm_test_comp_ = cyclus::CompMapPtr(new cyclus::CompMap(cyclus::MASS));
      (*non_norm_test_comp_)[u235_]=test_size_*one_g_;

      diff_comp_ = cyclus::CompMapPtr(new cyclus::CompMap(cyclus::MASS));
      (*diff_comp_)[u235_]=one_g_;
      (*diff_comp_)[pb208_]=one_g_;
      (*diff_comp_)[am241_]=one_g_;
      (*diff_comp_).normalize();

      // uranium and americium mat set up
      u_am_comp_= cyclus::CompMapPtr(new cyclus::CompMap(cyclus::MASS));
      (*u_am_comp_)[u235_] = one_;
      (*u_am_comp_)[am241_] = half_;

      // plutonium composition set up
      pu_comp_= cyclus::CompMapPtr(new cyclus::CompMap(cyclus::MASS));
      (*pu_comp_)[pu239_] = one_;

      // material creation
      test_mat_ = cyclus::Material::Ptr(new cyclus::Material(test_comp_));
      test_mat_->SetQuantity(test_size_);

      two_test_mat_ = cyclus::Material::Ptr(new cyclus::Material(test_comp_));
      two_test_mat_->SetQuantity(2*test_size_);

      ten_test_mat_ = cyclus::Material::Ptr(new cyclus::Material(test_comp_));
      ten_test_mat_->SetQuantity(10*test_size_);
      diff_mat_ = cyclus::Material::Ptr(new cyclus::Material(diff_comp_));
      diff_mat_->SetQuantity(test_size_);

      default_mat_ = cyclus::Material::Ptr(new cyclus::Material());
      u_am_mat_ = cyclus::Material::Ptr(new cyclus::Material(u_am_comp_));
      u_am_mat_->SetQuantity(test_size_);

      pu_mat_ = cyclus::Material::Ptr(new cyclus::Material(pu_comp_));
      pu_mat_->SetQuantity(test_size_);

      // vec creation
      test_vec_[1]=1.0;
      test_vec_[2]=2.0;
      test_vec_[3]=3.0;

      // test info
      u235_g_per_mol_ = 235.044;
      u235_halflife_ = 8445600000; // approximate, in months
      th228_halflife_ = 2*11; // approximate, in months
      int time_ = cyclus::TI->time();
    }

    virtual void TearDown(){
    }
};
