#ifndef CYCLUS_BID_PORTFOLIO_H_
#define CYCLUS_BID_PORTFOLIO_H_

#include "capacity_constraint.h"
#include "facility_model.h"
#include "bid.h"

namespace cyclus {

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
  /// @brief default constructor
  BidPortfolio() : bidder_(NULL), commodity_("NO_COMMODITY_SET") { };
  
  /// @return the model associated with the portfolio. if no bids have
  /// been added, the bidder is NULL.
  const cyclus::FacilityModel* bidder() {
    return bidder_;
  };
    
  /// @return the commodity associated with the portfolio. if no bids have
  /// been added, the commodity is 'NO_COMMODITY_SET'.
  std::string commodity() {
    return commodity_;
  };

  /// @brief add a bid to the portfolio
  /// @param r the bid to add
  /// @throws if a bid is added from a different bidder than the original or if
  /// the bid commodity is different than the original
  void AddResponse(const cyclus::Bid<T>& r) {
    VerifyResponder(r);
    VerifyCommodity(r);
    bids_.insert(r);
  };

  /// @brief add a capacity constraint associated with the portfolio
  /// @param c the constraint to add
  void AddConstraint(const cyclus::CapacityConstraint<T>& c) {
    constraints_.insert(c);
  };

  /// @return const access to the bids
  const std::set< cyclus::Bid<T> >& bids() {
    return bids_;
  };
  
  /// @return the set of constraints over the bids
  const std::set< cyclus::CapacityConstraint<T> >& constraints() {
    return constraints_;
  };

 private:
  /// @brief if the bidder has not been determined yet, it is set. otherwise
  /// VerifyResponder() verifies the the bid is associated with the
  /// portfolio's bidder
  /// @throws if a bid is added from a different bidder than the original
  void VerifyResponder(const cyclus::Bid<T> r) {
    if (bidder_ == NULL) {
      bidder_ = r.bidder;
    } else if (bidder_ != r.bidder) {
      std::string msg = "Can't insert a bid from " + r.bidder->name()
          + " into " + bidder_->name() + "'s portfolio.";
      throw cyclus::KeyError(msg);
    }
  };

  /// @brief if the commodity has not been determined yet, it is set. otherwise
  /// VerifyCommodity() verifies the the commodity is associated with the
  /// portfolio's commodity
  /// @throws if a commodity is added that is a different commodity from the
  /// original
  void VerifyCommodity(const cyclus::Bid<T> r) {
    std::string other = r.request->commodity;
    if (commodity_ == "NO_COMMODITY_SET") {
      commodity_ = other;
    } else if (commodity_ != other) {
      std::string msg = "Commodity mismatch for a request bid: "
                        + other + " != " + commodity_ + ".";
      throw cyclus::KeyError(msg);
    }
  };
  
  /// requests_ is a set because there is a one-to-one correspondance between a
  /// bid and a request, i.e., bids are unique
  std::set< cyclus::Bid<T> > bids_;

  /// constraints_ is a set because constraints are assumed to be unique
  std::set< cyclus::CapacityConstraint<T> > constraints_;
  
  std::string commodity_;
  cyclus::FacilityModel* bidder_;
};

} // namespace cyclus

#endif
