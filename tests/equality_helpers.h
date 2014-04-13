#ifndef CYCLUS_TESTS_EQUALITY_HELPERS_H_
#define CYCLUS_TESTS_EQUALITY_HELPERS_H_

#include <gtest/gtest.h>

#include "bid_portfolio.h"
#include "request_portfolio.h"

template<class T>
inline void TestVecEq(const std::vector<T>& lhs,
                      const std::vector<T>& rhs) {
  ASSERT_EQ(lhs.size(), rhs.size()) << "Vectors not the same size";
  for (int i = 0; i < lhs.size(); i++) {
    EXPECT_EQ(lhs[i], rhs[i]) << "Vectors differ at index " << i;
  }
}

template<>
inline void TestVecEq(const std::vector<double>& lhs,
                      const std::vector<double>& rhs) {
  ASSERT_EQ(lhs.size(), rhs.size()) << "Vectors not the same size";
  for (int i = 0; i < lhs.size(); i++) {
    EXPECT_DOUBLE_EQ(lhs[i], rhs[i]) << "Vectors differ at index " << i;
  }
}

template<>
inline void TestVecEq(const std::vector<float>& lhs,
                      const std::vector<float>& rhs) {
  ASSERT_EQ(lhs.size(), rhs.size()) << "Vectors not the same size";
  for (int i = 0; i < lhs.size(); i++) {
    EXPECT_FLOAT_EQ(lhs[i], rhs[i]) << "Vectors differ at index " << i;
  }
}

/// @brief BidPortfolio-BidPortfolio equality
/// @warning only tests bid set sizes are the same... sets are non optimal for
/// testing equality among members that have a single field (i.e., porfolio)
/// different
template<class T>
inline bool BPEq(const cyclus::BidPortfolio<T>& lhs,
                 const cyclus::BidPortfolio<T>& rhs) {
  return  (lhs.bids().size() == rhs.bids().size() &&
           lhs.constraints() == rhs.constraints() &&
           lhs.commodity() == rhs.commodity() &&
           lhs.bidder() == rhs.bidder());
};

/// @brief RequestPortfolio-RequestPortfolio equality
template<class T>
inline bool RPEq(const cyclus::RequestPortfolio<T>& lhs,
                 const cyclus::RequestPortfolio<T>& rhs) {
  return  (lhs.requests() == rhs.requests() &&
           lhs.constraints() == rhs.constraints() &&
           lhs.qty() == rhs.qty() &&
           lhs.requester() == rhs.requester());
};

// -----------
// Copied mostly from:
// http://stackoverflow.com/questions/1460703/comparison-of-arrays-in-google-test
template <class T>
void array_eq(const T* const expected, const T* const actual,
              unsigned long length) {
  for (unsigned long index = 0; index < length; index++) {
    T exp = expected[index];
    T act = actual[index];
    EXPECT_EQ(exp, act) << "arrays differ at index "
                        << index << "\n"
                        << " exp: " << exp << "\n"
                        << " act: " << act << "\n";
  }
}

template <class T>
void array_double_eq(const T* const expected, const T* const actual,
                     unsigned long length, std::string name = "") {
  for (unsigned long index = 0; index < length; index++) {
    T exp = expected[index];
    T act = actual[index];
    EXPECT_DOUBLE_EQ(exp, act) << name << " arrays differ at index "
                               << index << "\n"
                               << " exp: " << exp << "\n"
                               << " act: " << act << "\n";
  }
}

template <class T>
void pair_double_eq(const std::pair<T, double>& p1,
                    const std::pair<T, double>& p2) {
  EXPECT_EQ(p1.first, p2.first);
  EXPECT_DOUBLE_EQ(p1.second, p2.second);
}

template <class T>
void pair_double_eq(const std::pair<double, T>& p1,
                    const std::pair<double, T>& p2) {
  EXPECT_EQ(p1.second, p2.second);
  EXPECT_DOUBLE_EQ(p1.first, p2.first);
}

// -----------

#endif  // CYCLUS_TESTS_EQUALITY_HELPERS_H_
