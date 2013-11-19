#ifndef CYCLUS_REQUEST_PORTFOLIO_H_
#define CYCLUS_REQUEST_PORTFOLIO_H_

#include <set>
#include <string>
#include <vector>

#include "capacity_constraint.h"
#include "error.h"
#include "request.h"

namespace cyclus {

class Trader;
  
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
  RequestPortfolio() : requester_(NULL), qty_(-1), id_(next_id_++) { };
  
  /// @return the model associated with the portfolio. if no reqeusts have
  /// been added, the requester is NULL.
  const Trader* requester() const {
    return requester_;
  };

  /// @return the request quantity associated with the portfolio.
  double qty() {
    return qty_;
  };

  /// @return const access to the unconstrained requests
  //const std::vector<boost::shared_ptr< Request<T> > >& requests() const {
  const std::vector<typename Request<T>::Ptr>& requests() const {
    return requests_;
  };
  
  /// @brief add a request to the portfolio
  /// @param r the request to add
  /// @throws if a request is added from a different requester than the original
  void AddRequest(const typename Request<T>::Ptr r) {
    VerifyRequester(r);
    //VerifyQty(r);
    requests_.push_back(r);
    r->portfolio = this;
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

  /// @brief clear all associated containers
  void Clear() {
    requests_.clear();
    constraints_.clear();
  }
  
 private:  
  /// @brief if the requester has not been determined yet, it is set. otherwise
  /// VerifyRequester() verifies the the request is associated with the portfolio's
  /// requester
  /// @throws if a request is added from a different requester than the original
  void VerifyRequester(const typename Request<T>::Ptr r) {
    if (requester_ == NULL) {
      requester_ = r->requester;
    } else if (requester_ != r->requester) {
      std::string msg = "Insertion error: requesters do not match.";
      throw KeyError(msg);
    }
  };

  /// @brief if the quanityt has not been determined yet, it is set. otherwise
  /// VerifyRequester() verifies the the quantity is the same as all others in
  /// the portfolio
  /// @throws if a quanityt is different than the original
  void VerifyQty(const typename Request<T>::Ptr r) {
    double qty = r->target->quantity();
    if (qty_ == -1) {
      qty_ = qty;
    } else if (qty_ != qty) {
      std::string msg = "Insertion error: request quantity do not match.";
      throw KeyError(msg);
    }
  };

  /// requests_ is a vector because many requests may be identical, i.e., a set
  /// is not appropriate
  std::vector<typename Request<T>::Ptr> requests_;

  /// constraints_ is a set because constraints are assumed to be unique
  std::set< CapacityConstraint<T> > constraints_;

  double qty_;
  Trader* requester_;
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

/// @brief equality operator, allows comparison
template<class T>
bool operator==(const RequestPortfolio<T>& lhs,
                const RequestPortfolio<T>& rhs) {
  return  (lhs.requests() == rhs.requests() &&
           lhs.constraints() == rhs.constraints() &&
           lhs.requester() == rhs.requester());
};

/// @brief inequality operator
template<class T>
bool operator!=(const RequestPortfolio<T>& lhs,
                const RequestPortfolio<T>& rhs) {
  return !(lhs == rhs);
};

} // namespace cyclus

#endif
