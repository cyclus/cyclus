#ifndef CYCLUS_TESTS_EQUALITY_HELPERS_H_
#define CYCLUS_TESTS_EQUALITY_HELPERS_H_

#include "bid.h"
#include "bid_portfolio.h" 
#include "request_portfolio.h" 

/// 
/// template<class T>
/// inline bool BidEq(typename cyclus::Bid<T>::Ptr l,
///                   typename cyclus::Bid<T>::Ptr r) {
///   return l->request() == r->request() &&
///       l->offer() == r->offer() &&
///       l->bidder() == r->bidder();
/// }

/// /// 
/// template<class T>
/// inline bool BidVecPairEq(
///     const std::vector< std::pair< typename cyclus::Bid<T>::Ptr, double > >& l,
///     const std::vector< std::pair< typename cyclus::Bid<T>::Ptr, double > >& r) {
///   if (l.size() != r.size()) return false;
///   for (int i = 0; i < l.size(); i++) {
///     if (!(BidEq<T>(l[i].first, r[i].first) && l[i].second == l[i].second)) {
///       return false;
///     }
///   }
///   return true;
/// }

/// /// /// 
/// /// template<class T>
/// /// inline bool PrefMapEq(const typename PrefMap<T>::type& l,
/// ///                       const typename PrefMap<T>::type& r) {
/// /// }

/// /// 
/// template<class T>
/// inline bool BidVecEq(const std::vector<typename cyclus::Bid<T>::Ptr>& l,
///                      const std::vector<typename cyclus::Bid<T>::Ptr>& r) {
///   if (l.size() != r.size()) return false;
///   for (int i = 0; i < l.size(); i++) {
///     if (!(BidEq<T>(l[i], r[i]))) return false;
///   }
///   return true;
/// }

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
