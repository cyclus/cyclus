// CycArithmeticTests.cpp
#include <map>
#include <gtest/gtest.h>

#include "Error.h"
#include "CycArithmetic.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class CycArithmeticTest : public ::testing::Test {
  protected:
    std::vector<double> empty_vec_;
    std::vector<double> extreme_vec_;
    std::vector<double> ten_to_one_vec_;
    std::vector<double> zeros_vec_;
    std::vector<double> ones_vec_;
    std::map<int, double> empty_map_;
    std::map<int, double> zeros_map_;
    std::map<int, double> ten_to_one_map_;
    std::map<int, double> extreme_map_;
    double ten_to_one_sum_;

    virtual void SetUp(){
      ten_to_one_sum_=0.0;
      for(int i=0; i<10; ++i){
        ones_vec_.push_back(1);
        zeros_vec_.push_back(0);
        zeros_map_.insert(make_pair(i,0));
        ten_to_one_vec_.push_back(10-i);
        ten_to_one_map_.insert(make_pair(10-i,10-i));
        ten_to_one_sum_+=10-i;
      }
      extreme_vec_.push_back(1e16);
      extreme_vec_.push_back(1e-16);
      extreme_vec_.push_back(1);
    }
    virtual void TearDown() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(CycArithmeticTest, KahanVsRegSummation){
  // 1.0 + (1e-16)*999 =  1.0000000000000999
  std::vector<double> vec_to_sum;
  double sum = 1.0;
  vec_to_sum.push_back(sum);
  for(int i=0; i<999; ++i){
    double to_add = 1e-16;
    vec_to_sum.push_back(1e-16);
    sum += to_add;
  }

  ASSERT_EQ(vec_to_sum.size(), 1000);

  ASSERT_TRUE(1.0000000000000999 == cyclus::CycArithmetic::KahanSum(vec_to_sum));
  ASSERT_FALSE(1.0000000000000999 == sum);

  // you can test this in other ways : 
  
  // this passes : 
  ASSERT_NEAR(1.0+999e-16, cyclus::CycArithmetic::KahanSum(vec_to_sum), 1e-16);
  // while this fails : 
  // ASSERT_NEAR(1.0+999e-16, sum, 1e-16);
  
  // this passes:
  ASSERT_DOUBLE_EQ(1.0000000000000999, cyclus::CycArithmetic::KahanSum(vec_to_sum));
  // while this fails:
  // ASSERT_DOUBLE_EQ(1.0000000000000999, sum);


}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(CycArithmeticTest, KahanSumZero){
  EXPECT_FLOAT_EQ(0, cyclus::CycArithmetic::KahanSum(zeros_vec_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(CycArithmeticTest, KahanSumEmpty){
  EXPECT_FLOAT_EQ(0, cyclus::CycArithmetic::KahanSum(empty_vec_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(CycArithmeticTest, KahanSumOrdinary){
  EXPECT_FLOAT_EQ(10, cyclus::CycArithmetic::KahanSum(ones_vec_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(CycArithmeticTest, KahanSumExtremeVals){
  EXPECT_FLOAT_EQ(1e16+1e-16+1, cyclus::CycArithmetic::KahanSum(extreme_vec_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(CycArithmeticTest, sortAscendingVecEmpty) {
  std::vector<double> sorted;
  EXPECT_NO_THROW(sorted = cyclus::CycArithmetic::sort_ascending(empty_vec_));
  EXPECT_TRUE(sorted.empty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(CycArithmeticTest, sortAscendingMapEmpty) {
  std::vector<double> sorted;
  EXPECT_NO_THROW(sorted = cyclus::CycArithmetic::sort_ascending(empty_map_));
  EXPECT_TRUE(sorted.empty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(CycArithmeticTest, sortAscendingVecZeros) {
  std::vector<double> sorted;
  EXPECT_NO_THROW(sorted = cyclus::CycArithmetic::sort_ascending(zeros_vec_));
  std::vector<double>::iterator it;
  EXPECT_FALSE(sorted.empty());
  for(it=sorted.begin(); it!=sorted.end(); ++it) { 
    EXPECT_FLOAT_EQ(0, *it);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(CycArithmeticTest, sortAscendingMapZeros) {
  std::vector<double> sorted;
  EXPECT_NO_THROW(sorted = cyclus::CycArithmetic::sort_ascending(zeros_map_));
  std::vector<double>::iterator it;
  EXPECT_FALSE(sorted.empty());
  for(it=sorted.begin(); it!=sorted.end(); ++it) { 
    EXPECT_FLOAT_EQ(0, *it);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(CycArithmeticTest, sortAscendingVec) {
  std::vector<double> sorted;
  EXPECT_NO_THROW(sorted = cyclus::CycArithmetic::sort_ascending(ten_to_one_vec_));
  std::vector<double>::iterator it;
  EXPECT_FALSE(sorted.empty());
  int i=1;
  for(it=sorted.begin(); it!=sorted.end(); ++it) { 
    EXPECT_FLOAT_EQ(i, *it);
    i++;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(CycArithmeticTest, sortAscendingMap) {
  vector<double> sorted;
  EXPECT_NO_THROW(sorted = cyclus::CycArithmetic::sort_ascending(ten_to_one_map_));
  vector<double>::iterator it;
  EXPECT_FALSE(sorted.empty());
  int i = 1;
  for(it=sorted.begin(); it!=sorted.end(); ++it) { 
    EXPECT_FLOAT_EQ(i, *it);
    i++;
  }
}

