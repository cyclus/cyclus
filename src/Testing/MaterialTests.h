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
    CompMapPtr test_comp_, diff_comp;
    double test_size_, fraction;
    mat_rsrc_ptr test_mat_;
    mat_rsrc_ptr diff_mat_;
    long int u235_halflife_;
    int th228_halflife_;

    virtual void SetUp(){
      // composition set up
      u235_ = 92235;
      am241_ = 95241;
      th228_ = 90228;
      pb208_ = 82208;
      one_g_ = 1.0;

      // composition creation
      test_comp_ = CompMapPtr(new CompMap(MASS));
      diff_comp = CompMapPtr(new CompMap(MASS));
      (*test_comp_)[u235_]=one_g_;
      (*diff_comp)[u235_]=one_g_;
      (*diff_comp)[pb208_]=one_g_;
      (*diff_comp)[am241_]=one_g_;
      test_size_ = 10.0;
      fraction = 2.0 / 3.0;
      

      // material creation
      test_mat_ = mat_rsrc_ptr(new Material(test_comp_));
      test_mat_->setQuantity(test_size_);
      diff_mat_ = mat_rsrc_ptr(new Material(diff_comp));
      diff_mat_->setQuantity(test_size_);

      // test info
      u235_halflife_ = 8445600000; // approximate, in months
      th228_halflife_ = 2*11; // approximate, in months
      int time_ = TI->time();
    }

    virtual void TearDown(){
    }
};
