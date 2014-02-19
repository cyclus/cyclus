// mass_table_tests.cc
#ifndef CYCLUS_TESTS_MASS_TABLE_TESTS_H_
#define CYCLUS_TESTS_MASS_TABLE_TESTS_H_

#include <gtest/gtest.h>
#include "mass_table.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class MassTableTest : public ::testing::Test {
  protected:
    int u235_, am241_, th228_, pb208_;
    double one_g_;

    virtual void SetUp(){
      // composition set up
      u235_ = 922350000;
      am241_ = 952410000;
      th228_ = 902280000;
      pb208_ = 822080000;
      one_g_ = 1.0;

    }

    virtual void TearDown(){
    }
};

#endif  // CYCLUS_TESTS_MASS_TABLE_TESTS_H_
