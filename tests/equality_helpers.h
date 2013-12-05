#ifndef CYCLUS_TESTS_EQUALITY_HELPERS_H_
#define CYCLUS_TESTS_EQUALITY_HELPERS_H_

#include "bid_portfolio.h" 
#include "request_portfolio.h" 

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

#endif // CYCLUS_TESTS_EQUALITY_HELPERS_H_
