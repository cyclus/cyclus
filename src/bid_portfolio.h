#ifndef CYCLUS_BID_PORTFOLIO_H_
#define CYCLUS_BID_PORTFOLIO_H_

#include "capacity_constraint.h"
#include "facility_model.h"
#include "bid.h"

namespace cyclus {

/// @class ResponsePortfolio
///
/// @brief A ResponsePortfolio is a collection of bids to requests for
/// resources and associated constraints on those bids.
///
/// A ResponsePortfolio contains all the information corresponding to a
/// responder to resource requests. It is a light wrapper around the set
/// of bids and constraints for a given responder, guaranteeing a single
/// responder per portfolio. Responses are grouped by both the responder and the
/// commodity that it produces. Constraints are assumed to act over the entire set
/// of possible bids.
template <class T>
class ResponsePortfolio {
 public:
  /// @brief default constructor
  ResponsePortfolio() : responder_(NULL), commodity_("NO_COMMODITY_SET") { };
  
  /// @return the model associated with the portfolio. if no bids have
  /// been added, the responder is NULL.
  const cyclus::FacilityModel* responder() {
    return responder_;
  };
    
  /// @return the commodity associated with the portfolio. if no bids have
  /// been added, the commodity is 'NO_COMMODITY_SET'.
  std::string commodity() {
    return commodity_;
  };

  /// @brief add a bid to the portfolio
  /// @param r the bid to add
  /// @throws if a bid is added from a different responder than the original
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
  /// @brief if the responder has not been determined yet, it is set. otherwise
  /// VerifyResponder() verifies the the bid is associated with the
  /// portfolio's responder
  /// @throws if a bid is added from a different responder than the original
  void VerifyResponder(const cyclus::Bid<T> r) {
    if (responder_ == NULL) {
      responder_ = r.responder;
    } else if (responder_ != r.responder) {
      std::string msg = "Can't insert a bid from " + r.responder->name()
          + " into " + responder_->name() + "'s portfolio.";
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
  cyclus::FacilityModel* responder_;
};

} // namespace cyclus

#endif
