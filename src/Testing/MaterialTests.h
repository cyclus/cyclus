// MaterialTests.cpp
#include <gtest/gtest.h>
#include "Material.h"
#include "GenericResource.h"
#include "IsoVector.h"
#include "Timer.h"
#include "CycException.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class MaterialTest : public ::testing::Test {
  protected:
    Iso u235_, am241_, th228_, pb208_, pu239_;
    int one_g_; // grams
    double one_; // unitless
    double half_; // unitless
    CompMapPtr test_comp_, two_test_comp_, non_norm_test_comp_, diff_comp_, 
               pu_comp_, u_am_comp_;
    double test_size_, fraction;
    mat_rsrc_ptr test_mat_, two_test_mat_, ten_test_mat_, pu_mat_, u_am_mat_;
    mat_rsrc_ptr diff_mat_;
    mat_rsrc_ptr default_mat_;
    long int u235_halflife_;
    int th228_halflife_;
    double u235_g_per_mol_;

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
      test_comp_ = CompMapPtr(new CompMap(MASS));
      (*test_comp_)[u235_]=one_g_;
      (*test_comp_).normalize();

      two_test_comp_ = CompMapPtr(new CompMap(MASS));
      (*two_test_comp_)[u235_]=one_g_;

      non_norm_test_comp_ = CompMapPtr(new CompMap(MASS));
      (*non_norm_test_comp_)[u235_]=test_size_*one_g_;

      diff_comp_ = CompMapPtr(new CompMap(MASS));
      (*diff_comp_)[u235_]=one_g_;
      (*diff_comp_)[pb208_]=one_g_;
      (*diff_comp_)[am241_]=one_g_;
      (*diff_comp_).normalize();

      // uranium and americium mat set up
      u_am_comp_= CompMapPtr(new CompMap(MASS));
      (*u_am_comp_)[u235_] = one_;
      (*u_am_comp_)[am241_] = half_;

      // plutonium composition set up
      pu_comp_= CompMapPtr(new CompMap(MASS));
      (*pu_comp_)[pu239_] = one_;

      // material creation
      test_mat_ = mat_rsrc_ptr(new Material(test_comp_));
      test_mat_->setQuantity(test_size_);

      two_test_mat_ = mat_rsrc_ptr(new Material(test_comp_));
      two_test_mat_->setQuantity(2*test_size_);

      ten_test_mat_ = mat_rsrc_ptr(new Material(test_comp_));
      ten_test_mat_->setQuantity(10*test_size_);
      diff_mat_ = mat_rsrc_ptr(new Material(diff_comp_));
      diff_mat_->setQuantity(test_size_);

      default_mat_ = mat_rsrc_ptr(new Material());
      u_am_mat_ = mat_rsrc_ptr(new Material(u_am_comp_));
      u_am_mat_->setQuantity(test_size_);

      pu_mat_ = mat_rsrc_ptr(new Material(pu_comp_));
      pu_mat_->setQuantity(test_size_);

      // test info
      u235_g_per_mol_ = 235.044;
      u235_halflife_ = 8445600000; // approximate, in months
      th228_halflife_ = 2*11; // approximate, in months
      int time_ = TI->time();
    }

    virtual void TearDown(){
    }
};
