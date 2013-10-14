#ifndef CYCLUS_REQUEST_PORTFOLIO_H_
#define CYCLUS_REQUEST_PORTFOLIO_H_

#include <set>
#include <string>

#include "capacity_constraint.h"
#include "error.h"
#include "facility_model.h"
#include "request.h"

namespace cyclus {
  
/// @class RequestPortfolio contains all the information corresponding to a
/// requester of resources in the exchange. It is a light wrapper around the set
/// of requests and constraints for a given requester, guaranteeing a single
/// requester per portfolio.
/// @TODO revise the request portfolio API/underlying data structures to allow
/// more than one type of constraint, specifically, exclusive constraints
template<class T>
class RequestPortfolio {
 public:
  /// @brief default constructor
  RequestPortfolio() : requester_(NULL) { };
  
  /// @brief add a request to the portfolio
  /// @param r the request to add
  /// @throws if a request is added from a different requester than the original
  void AddRequest(const cyclus::Request<T>& r) {
    VerifyRequester(r);
    requests_.push_back(r);
  };

  /// @brief add a capacity constraint associated with the portfolio, if it
  /// doesn't already exist
  /// @param c the constraint to add
  void AddCapacityConstraint(const cyclus::CapacityConstraint<T>& c) {
    if (constrained_requests_.count(c) == 0) {
      constrained_requests_.insert(std::make_pair(c,
                                                  std::vector<Request<T> >()));
    }
  };

  /// @brief a request that is associated with a capacity constraint
  /// @param r the request to add
  /// @param c the constraint the request is associated with
  /// @throws if a request is added from a different requester than the original
  void AddConstrainedRequest(const cyclus::Request<T>& r,
                             const cyclus::CapacityConstraint<T>& c) {
    VerifyRequester(r);
    if (constrained_requests_.count(c) == 0) {
      AddCapacityConstraint(c);
    }
    constrained_requests_[c].push_back(r);
  };
  
  /// @return the model associated with the portfolio. if no reqeusts have
  /// been added, the requester is NULL.
  const cyclus::FacilityModel* requester() {
    return requester_;
  };
  
  /// @return const access to the unconstrained requests
  const std::vector<cyclus::Request<T> >& requests() {
    return requests_;
  };
  
  /// @return const access to the constrained requests
  const std::map< cyclus::CapacityConstraint<T>,
      std::vector<cyclus::Request<T> > >& constrainted_requests() {
    return constrained_requests_;
  };

 private:  
  /// @brief if the requester has not been determined yet, it is set. otherwise
  /// VerifyRequester() verifies the the request is associated with the portfolio's
  /// requester
  /// @throws if a request is added from a different requester than the original
  void VerifyRequester(const cyclus::Request<T> r) {
    if (requester_ == NULL) {
      requester_ = r.requester;
    } else if (requester_ != r.requester) {
      std::string msg = "Can't insert a request from " + r.requester->name()
          + " into " + requester_->name() + "'s portfolio.";
      throw cyclus::KeyError(msg);
    }
  };

  /// @return the set of requests in the portfolio
  std::vector< Request<T> > requests_;

  /// @return the set of constraints over the requests based on request
  /// capacity
  std::map< cyclus::CapacityConstraint<T>, std::vector<cyclus::Request<T> > >
      constrained_requests_; 

  /// @return the requester associated with the portfolio
  cyclus::FacilityModel* requester_;
};

} // namespace cyclus

#endif
