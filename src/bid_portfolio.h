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
class BidPortfolio :
public boost::enable_shared_from_this< BidPortfolio<T> > {
 public:
  typedef boost::shared_ptr< BidPortfolio<T> > Ptr;

  /// @brief default constructor
  BidPortfolio()
    : bidder_(NULL),
      commodity_("NO_COMMODITY_SET"),
      id_(next_id_++) {
    constraints_.clear();
  };

  /// @brief add a bid to the portfolio
  /// @param r the bid to add
  /// @throws if a bid is added from a different bidder than the original or if
  /// the bid commodity is different than the original
  void AddBid(const typename Bid<T>::Ptr b) {
    VerifyResponder_(b);
    VerifyCommodity_(b);
    bids_.insert(b);
    b->set_portfolio(this->shared_from_this());
  };

  /// @brief add a capacity constraint associated with the portfolio
  /// @param c the constraint to add
  inline void AddConstraint(const CapacityConstraint<T>& c) {
    constraints_.insert(c);
  };

  /// @brief if the bidder has not been determined yet, it is set. otherwise
  /// VerifyResponder() verifies the the bid is associated with the
  /// portfolio's bidder
  /// @throws if a bid is added from a different bidder than the original
  void VerifyResponder_(typename Bid<T>::Ptr b) {
    if (bidder_ == NULL) {
      bidder_ = b->bidder();
    } else if (bidder_ != b->bidder()) {
      std::string msg = "Insertion error: bidders do not match.";
      throw KeyError(msg);
    }
  };

  /// @brief if the commodity has not been determined yet, it is set. otherwise
  /// VerifyCommodity() verifies the the commodity is associated with the
  /// portfolio's commodity
  /// @throws if a commodity is added that is a different commodity from the
  /// original
  void VerifyCommodity_(const typename Bid<T>::Ptr r) {
    std::string other = r->request()->commodity();
    if (commodity_ == "NO_COMMODITY_SET") {
      commodity_ = other;
    } else if (commodity_ != other) {
      std::string msg = "Insertion error: commodities do not match.";
      throw KeyError(msg);
    }
  };
  
  /// @return the model associated with the portfolio. if no bids have
  /// been added, the bidder is NULL.
  inline Trader* bidder() const {
    return bidder_;
  };
    
  /// @return the commodity associated with the portfolio. if no bids have
  /// been added, the commodity is 'NO_COMMODITY_SET'.
  inline std::string commodity() const {
    return commodity_;
  };

  /// @return const access to the bids
  inline const std::set<typename Bid<T>::Ptr>& bids() const {
    return bids_;
  };
  
  /// @return the set of constraints over the bids
  inline const std::set< CapacityConstraint<T> >& constraints() const {
    return constraints_;
  };

  /// @return a unique id for the constraint
  inline const int id() const {
    return id_;
  }
  
  // bid_ is a set because there is a one-to-one correspondance between a
  // bid and a request, i.e., bids are unique
  std::set< typename Bid<T>::Ptr > bids_;

  // constraints_ is a set because constraints are assumed to be unique
  std::set< CapacityConstraint<T> > constraints_;
  
  std::string commodity_;
  Trader* bidder_;
  int id_;
  static int next_id_;

 private:
  /// @brief copy constructor, which we wish not to be used in general, due to
  /// the ownership relation of the bids
  BidPortfolio(const BidPortfolio& rhs) : id_(next_id_++) {
    bidder_ = rhs.bidder_;
    bids_ = rhs.bids_;
    commodity_ = rhs.commodity_;
    constraints_ = rhs.constraints_;
    typename std::set<typename Bid<T>::Ptr>::iterator it;
    for (it = bids_.begin(); it != bids_.end(); ++it) {
      it->get()->set_portfolio(this->shared_from_this());
    }
  };
};

template<class T> int BidPortfolio<T>::next_id_ = 0;

/// @brief BidPortfolio-BidPortfolio comparison operator, allows usage in
/// ordered containers
template<class T>
inline bool operator<(const BidPortfolio<T>& lhs,
                      const BidPortfolio<T>& rhs) {
  return  (lhs.id() < rhs.id());
};

/// @brief BidPortfolio-BidPortfolio equality operator
template<class T>
inline bool operator==(const BidPortfolio<T>& lhs,
                       const BidPortfolio<T>& rhs) {
  return  (lhs.bids() == rhs.bids() &&
           lhs.constraints() == rhs.constraints() &&
           lhs.commodity() == rhs.commodity() &&
           lhs.bidder() == rhs.bidder());
};

/// @brief BidPortfolio-BidPortfolio inequality operator
template<class T>
inline bool operator!=(const BidPortfolio<T>& lhs,
                       const BidPortfolio<T>& rhs) {
  return !(lhs == rhs);
};

} // namespace cyclus

#endif
