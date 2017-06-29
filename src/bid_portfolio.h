#ifndef CYCLUS_SRC_BID_PORTFOLIO_H_
#define CYCLUS_SRC_BID_PORTFOLIO_H_

#include <set>
#include <sstream>
#include <string>

#include <boost/shared_ptr.hpp>

#include "bid.h"
#include "capacity_constraint.h"
#include "error.h"

namespace cyclus {

class Trader;

std::string GetTraderPrototype(Trader* bidder);
std::string GetTraderSpec(Trader* bidder);

/// @class BidPortfolio
///
/// @brief A BidPortfolio is a collection of bids as responses to requests for
/// resources and associated constraints on those bids.
///
/// A BidPortfolio contains all the information corresponding to a bidder's
/// response to resource requests. It is a light wrapper around the set of bids
/// and constraints for a given bidder, guaranteeing a single bidder per
/// portfolio. Responses are grouped by the bidder. Constraints are assumed to
/// act over the entire set of possible bids.
template <class T>
class BidPortfolio : public boost::enable_shared_from_this<BidPortfolio<T>> {
 public:
  typedef boost::shared_ptr<BidPortfolio<T>> Ptr;

  /// @brief default constructor
  BidPortfolio() : bidder_(NULL) {}

  /// deletes all bids associated with it
  ~BidPortfolio() {
    typename std::set<Bid<T>*>::iterator it;
    for (it = bids_.begin(); it != bids_.end(); ++it) {
      delete *it;
    }
  }

  /// @brief add a bid to the portfolio
  /// @param request the request being responded to by this bid
  /// @param offer the resource being offered in response to the request
  /// @param bidder the bidder
  /// @param exclusive indicates whether the bid is exclusive
  /// @param preference sets the preference of the bid on a request
  ///        bid arc.
  /// @throws KeyError if a bid is added from a different bidder than the
  /// original
  Bid<T>* AddBid(Request<T>* request, boost::shared_ptr<T> offer,
                 Trader* bidder, bool exclusive, double preference) {
    Bid<T>* b = Bid<T>::Create(request, offer, bidder, this->shared_from_this(),
                               exclusive, preference);
    VerifyResponder_(b);
    if (offer->quantity() > 0)
      bids_.insert(b);
    else {
      std::stringstream ss;
      ss << GetTraderPrototype(bidder) << " from " << GetTraderSpec(bidder)
         << " is offering a bid quantity <= 0, Q = " << offer->quantity();
      throw ValueError(ss.str());
    }
    return b;
  }

  /// @brief add a bid to the portfolio
  /// @param request the request being responded to by this bid
  /// @param offer the resource being offered in response to the request
  /// @param bidder the bidder
  /// @param exclusive indicates whether the bid is exclusive
  /// @throws KeyError if a bid is added from a different bidder than the
  /// original
  Bid<T>* AddBid(Request<T>* request, boost::shared_ptr<T> offer,
                 Trader* bidder, bool exclusive = false) {
    return AddBid(request, offer, bidder, exclusive,
                  std::numeric_limits<double>::quiet_NaN());
  }

  /// @brief add a capacity constraint associated with the portfolio
  /// @param c the constraint to add
  inline void AddConstraint(const CapacityConstraint<T>& c) {
    constraints_.insert(c);
  }

  /// @return the agent associated with the portfolio. If no bids have
  /// been added, the bidder is NULL.
  inline Trader* bidder() const { return bidder_; }

  /// @return *deprecated* the commodity associated with the portfolio.
  inline std::string commodity() const { return ""; }

  /// @return const access to the bids
  inline const std::set<Bid<T>*>& bids() const { return bids_; }

  /// @return the set of constraints over the bids
  inline const std::set<CapacityConstraint<T>>& constraints() const {
    return constraints_;
  }

 private:
  /// @brief copy constructor is private to prevent copying and preserve
  /// explicit single-ownership of bids
  BidPortfolio(const BidPortfolio& rhs) {
    bidder_ = rhs.bidder_;
    bids_ = rhs.bids_;
    constraints_ = rhs.constraints_;
    typename std::set<Bid<T>*>::iterator it;
    for (it = bids_.begin(); it != bids_.end(); ++it) {
      it->get()->set_portfolio(this->shared_from_this());
    }
  }

  /// @brief if the bidder has not been determined yet, it is set. Otherwise
  /// VerifyResponder() verifies the bid is associated with the
  /// portfolio's bidder
  /// @throws KeyError if a bid is added from a different bidder than the
  /// original
  void VerifyResponder_(Bid<T>* b) {
    if (bidder_ == NULL) {
      bidder_ = b->bidder();
    } else if (bidder_ != b->bidder()) {
      std::string msg = "Insertion error: bidders do not match.";
      throw KeyError(msg);
    }
  }

  /// @brief *deprecated*
  void VerifyCommodity_(const Bid<T>* r) {}

  // bid_ is a set because there is a one-to-one correspondence between a
  // bid and a request, i.e., bids are unique
  std::set<Bid<T>*> bids_;

  // constraints_ is a set because constraints are assumed to be unique
  std::set<CapacityConstraint<T>> constraints_;

  Trader* bidder_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_BID_PORTFOLIO_H_
