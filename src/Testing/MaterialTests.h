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
    Iso u235_, am241_, th228_, pb208_;
    int one_g_; // grams
    CompMapPtr test_comp_, two_test_comp_, non_norm_test_comp_, diff_comp_;
    double test_size_, fraction;
    mat_rsrc_ptr test_mat_, two_test_mat_, ten_test_mat_;
    mat_rsrc_ptr diff_mat_;
    mat_rsrc_ptr default_mat_;
    long int u235_halflife_;
    int th228_halflife_;
    double u235_g_per_mol_;
    std::map<Iso, double> test_vec_;

    virtual void SetUp(){
      // composition set up
      u235_ = 92235;
      am241_ = 95241;
      th228_ = 90228;
      pb208_ = 82208;
      one_g_ = 1.0;
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

      // vec creation
      test_vec_[1]=1.0;
      test_vec_[2]=2.0;
      test_vec_[3]=3.0;

      // test info
      u235_g_per_mol_ = 235.044;
      u235_halflife_ = 8445600000; // approximate, in months
      th228_halflife_ = 2*11; // approximate, in months
      int time_ = TI->time();
    }

    virtual void TearDown(){
    }
};
