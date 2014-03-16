#ifndef CYCLUS_SRC_BID_PORTFOLIO_H_
#define CYCLUS_SRC_BID_PORTFOLIO_H_

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
class BidPortfolio : public boost::enable_shared_from_this< BidPortfolio<T> > {
 public:
  typedef boost::shared_ptr< BidPortfolio<T> > Ptr;

  /// @brief default constructor
  BidPortfolio()
      : bidder_(NULL),
        commodity_("NO_COMMODITY_SET") {}

  /// @brief add a bid to the portfolio
  /// @param request the request being responded to by this bid
  /// @param offer the resource being offered in response to the request
  /// @param bidder the bidder
  /// @throws KeyError if a bid is added from a different bidder than the
  /// original or if the bid commodity is different than the original
  typename Bid<T>::Ptr AddBid(typename Request<T>::Ptr request,
                              boost::shared_ptr<T> offer,
                              Trader* bidder,
                              bool exclusive = false) {
    typename Bid<T>::Ptr b =
        Bid<T>::Create(request, offer, bidder, this->shared_from_this(),
                       exclusive);
    VerifyResponder_(b);
    VerifyCommodity_(b);
    bids_.insert(b);
    return b;
  }

  /// @brief add a capacity constraint associated with the portfolio
  /// @param c the constraint to add
  inline void AddConstraint(const CapacityConstraint<T>& c) {
    constraints_.insert(c);
  }

  /// @return the model associated with the portfolio. If no bids have
  /// been added, the bidder is NULL.
  inline Trader* bidder() const {
    return bidder_;
  }

  /// @return the commodity associated with the portfolio. If no bids have
  /// been added, the commodity is 'NO_COMMODITY_SET'.
  inline std::string commodity() const {
    return commodity_;
  }

  /// @return const access to the bids
  inline const std::set<typename Bid<T>::Ptr>& bids() const {
    return bids_;
  }

  /// @return the set of constraints over the bids
  inline const std::set< CapacityConstraint<T> >& constraints() const {
    return constraints_;
  }

 private:
  /// @brief copy constructor is private to prevent copying and preserve
  /// explicit single-ownership of bids
  BidPortfolio(const BidPortfolio& rhs) {
    bidder_ = rhs.bidder_;
    bids_ = rhs.bids_;
    commodity_ = rhs.commodity_;
    constraints_ = rhs.constraints_;
    typename std::set<typename Bid<T>::Ptr>::iterator it;
    for (it = bids_.begin(); it != bids_.end(); ++it) {
      it->get()->set_portfolio(this->shared_from_this());
    }
  }

  /// @brief if the bidder has not been determined yet, it is set. Otherwise
  /// VerifyResponder() verifies the bid is associated with the
  /// portfolio's bidder
  /// @throws KeyError if a bid is added from a different bidder than the
  /// original
  void VerifyResponder_(typename Bid<T>::Ptr b) {
    if (bidder_ == NULL) {
      bidder_ = b->bidder();
    } else if (bidder_ != b->bidder()) {
      std::string msg = "Insertion error: bidders do not match.";
      throw KeyError(msg);
    }
  }

  /// @brief if the commodity has not been determined yet, it is set. Otherwise
  /// VerifyCommodity() verifies the commodity associated with the
  /// portfolio's commodity
  /// @throws KeyError if a commodity is added that is a different commodity
  /// from the original
  void VerifyCommodity_(const typename Bid<T>::Ptr r) {
    std::string other = r->request()->commodity();
    if (commodity_ == "NO_COMMODITY_SET") {
      commodity_ = other;
    } else if (commodity_ != other) {
      std::string msg = "Insertion error: commodities do not match.";
      throw KeyError(msg);
    }
  }

  // bid_ is a set because there is a one-to-one correspondence between a
  // bid and a request, i.e., bids are unique
  std::set< typename Bid<T>::Ptr > bids_;

  // constraints_ is a set because constraints are assumed to be unique
  std::set< CapacityConstraint<T> > constraints_;

  std::string commodity_;
  Trader* bidder_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_BID_PORTFOLIO_H_
