#ifndef CYCLUS_BID_PORTFOLIO_H_
#define CYCLUS_BID_PORTFOLIO_H_

#include <set>
#include <string>

#include <boost/shared_ptr.hpp>

#include "bid.h"
#include "capacity_constraint.h"
#include "error.h"

namespace cyclus {

class Trader;

/// @class BidPortfolio
///
/// @brief A BidPortfolio is a collection of bids as responses to requests for
/// resources and associated constraints on those bids.
///
/// A BidPortfolio contains all the information corresponding to a
/// bidder's response to resource requests. It is a light wrapper around the set
/// of bids and constraints for a given bidder, guaranteeing a single
/// bidder and commodity per portfolio. Responses are grouped by both the bidder
/// and the commodity that it produces. Constraints are assumed to act over the
/// entire set of possible bids.
template <class T>
class BidPortfolio {
 public:
  typedef boost::shared_ptr< BidPortfolio<T> > Ptr;

  /// @brief default constructor
  BidPortfolio()
      : bidder_(NULL),
        commodity_("NO_COMMODITY_SET"),
        id_(next_id_++) { };
  
  /// @return the model associated with the portfolio. if no bids have
  /// been added, the bidder is NULL.
  Trader* bidder() const {
    return bidder_;
  };
    
  /// @return the commodity associated with the portfolio. if no bids have
  /// been added, the commodity is 'NO_COMMODITY_SET'.
  std::string commodity() const {
    return commodity_;
  };

  /// @brief add a bid to the portfolio
  /// @param r the bid to add
  /// @throws if a bid is added from a different bidder than the original or if
  /// the bid commodity is different than the original
  void AddBid(const typename Bid<T>::Ptr b) {
    VerifyResponder(b);
    VerifyCommodity(b);
    bids_.insert(b);
    b->portfolio = this;
  };

  /// @brief add a capacity constraint associated with the portfolio
  /// @param c the constraint to add
  void AddConstraint(const CapacityConstraint<T>& c) {
    constraints_.insert(c);
  };

  /// @return const access to the bids
  const std::set<typename Bid<T>::Ptr>& bids() const {
    return bids_;
  };
  
  /// @return the set of constraints over the bids
  const std::set< CapacityConstraint<T> >& constraints() const {
    return constraints_;
  };

  /// @return a unique id for the constraint
  const int id() const {return id_;};

  /// @brief clear all associated containers
  void Clear() {
    bids_.clear();
    constraints_.clear();
  }

 private:
  /// @brief if the bidder has not been determined yet, it is set. otherwise
  /// VerifyResponder() verifies the the bid is associated with the
  /// portfolio's bidder
  /// @throws if a bid is added from a different bidder than the original
  void VerifyResponder(const typename Bid<T>::Ptr r) {
    if (bidder_ == NULL) {
      bidder_ = r->bidder;
    } else if (bidder_ != r->bidder) {
      std::string msg = "Insertion error: bidders do not match.";
      throw KeyError(msg);
    }
  };

  /// @brief if the commodity has not been determined yet, it is set. otherwise
  /// VerifyCommodity() verifies the the commodity is associated with the
  /// portfolio's commodity
  /// @throws if a commodity is added that is a different commodity from the
  /// original
  void VerifyCommodity(const typename Bid<T>::Ptr r) {
    std::string other = r->request->commodity;
    if (commodity_ == "NO_COMMODITY_SET") {
      commodity_ = other;
    } else if (commodity_ != other) {
      std::string msg = "Insertion error: commodities do not match.";
      throw KeyError(msg);
    }
  };
  
  /// bid_ is a set because there is a one-to-one correspondance between a
  /// bid and a request, i.e., bids are unique
  std::set< typename Bid<T>::Ptr > bids_;

  /// constraints_ is a set because constraints are assumed to be unique
  std::set< CapacityConstraint<T> > constraints_;
  
  std::string commodity_;
  Trader* bidder_;
  int id_;
  static int next_id_;
};

template<class T> int BidPortfolio<T>::next_id_ = 0;

/// @brief comparison operator, allows usage in ordered containers
template<class T>
bool operator<(const BidPortfolio<T>& lhs,
               const BidPortfolio<T>& rhs) {
  return  (lhs.id() < rhs.id());
};

/// @brief equality operator, allows comparison
template<class T>
bool operator==(const BidPortfolio<T>& lhs,
                const BidPortfolio<T>& rhs) {
  return  (lhs.bids() == rhs.bids() &&
           lhs.constraints() == rhs.constraints() &&
           lhs.commodity() == rhs.commodity() &&
           lhs.bidder() == rhs.bidder());
};

/// @brief inequality operator
template<class T>
bool operator!=(const BidPortfolio<T>& lhs,
                const BidPortfolio<T>& rhs) {
  return !(lhs == rhs);
};

} // namespace cyclus

#endif
