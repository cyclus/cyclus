#ifndef CYCLUS_REQUEST_PORTFOLIO_H_
#define CYCLUS_REQUEST_PORTFOLIO_H_

#include <set>
#include <string>
#include <vector>

#include "capacity_constraint.h"
#include "error.h"
#include "request.h"

namespace cyclus {

class Exchanger;
  
/// @class RequestPortfolio
/// 
/// @brief A RequestPortfolio is a group of (possibly constrainted) requests for
/// resources
///
/// The portfolio contains a grouping of resource requests that may be mutually
/// met by suppliers. These requests may share a common set of
/// constraints. Take, for instance, a facility that needs fuel, of which there are
/// two commodity types, fuelA and fuelB. If some combination of the two suffice
/// the facility's needs, then requests for both would be added to the portfolio
/// along with a capacity constraint.
///
/// @TODO revise the request portfolio API/underlying data structures to allow
/// exclusive constraints. Perhaps the easiest option would be to allow the
/// AddRequest interface have an option boolean for exclusivity. This utility needs
/// to be used first to determine what the appropriate way forward is.
template<class T>
class RequestPortfolio {
 public:
  /// @brief default constructor
  RequestPortfolio() : requester_(NULL) { };
  
  /// @return the model associated with the portfolio. if no reqeusts have
  /// been added, the requester is NULL.
  const Exchanger* requester() const {
    return requester_;
  };
  
  /// @return const access to the unconstrained requests
  const std::vector< Request<T> >& requests() const {
    return requests_;
  };
  
  /// @brief add a request to the portfolio
  /// @param r the request to add
  /// @throws if a request is added from a different requester than the original
  void AddRequest(const Request<T>& r) {
    VerifyRequester(r);
    requests_.push_back(r);
  };

  /// @brief add a capacity constraint associated with the portfolio, if it
  /// doesn't already exist
  /// @param c the constraint to add
  void AddConstraint(const CapacityConstraint<T>& c) {
    constraints_.insert(c);
  };
  
  /// @return const access to the request constraints
  const std::set< CapacityConstraint<T> >& constraints() const {
    return constraints_;
  };

  /// @return a unique id for the constraint
  const int id() const {return id_;};

 private:  
  /// @brief if the requester has not been determined yet, it is set. otherwise
  /// VerifyRequester() verifies the the request is associated with the portfolio's
  /// requester
  /// @throws if a request is added from a different requester than the original
  void VerifyRequester(const Request<T> r) {
    if (requester_ == NULL) {
      requester_ = r.requester;
    } else if (requester_ != r.requester) {
      std::string msg = "Insertion error: requesters do not match.";
      throw KeyError(msg);
    }
  };

  /// requests_ is a vector because many requests may be identical, i.e., a set
  /// is not appropriate
  std::vector< Request<T> > requests_;

  /// constraints_ is a set because constraints are assumed to be unique
  std::set< CapacityConstraint<T> > constraints_;
  
  Exchanger* requester_;
  int id_;
  static int next_id_;
};

template<class T> int RequestPortfolio<T>::next_id_ = 0;

/// @brief comparison operator, allows usage in ordered containers
template<class T>
bool operator<(const RequestPortfolio<T>& lhs,
               const RequestPortfolio<T>& rhs) {
  return  (lhs.id() < rhs.id());
};

} // namespace cyclus

#endif
