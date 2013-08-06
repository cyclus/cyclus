// mass_table_tests.cc
#include <gtest/gtest.h>
#include "mass_table.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class MassTableTest : public ::testing::Test {
  protected:
    int u235_, am241_, th228_, pb208_;
    double one_g_;

    virtual void SetUp(){
      // composition set up
      u235_ = 92235;
      am241_ = 95241;
      th228_ = 90228;
      pb208_ = 82208;
      one_g_ = 1.0;

    }

    virtual void TearDown(){
    }
};
