#ifndef CYCLUS_SRC_REQUEST_PORTFOLIO_H_
#define CYCLUS_SRC_REQUEST_PORTFOLIO_H_

#include <functional>
#include <numeric>
#include <set>
#include <string>
#include <vector>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include "capacity_constraint.h"
#include "error.h"
#include "logger.h"
#include "request.h"

namespace cyclus {

class Trader;

/// @brief accumulator sum for request quantities
template <class T> inline double SumQty(double total, Request<T>* r) {
  return total += r->target()->quantity();
}

/// @brief provide coefficients for default mass constraints
///
/// Coefficients are determiend by the request portfolio and are provided to the
/// converter. The arc and exchange context are used in order to reference the
/// original request so that the request's coefficient can be applied.
template <class T> struct QtyCoeffConverter : public Converter<T> {
  QtyCoeffConverter(const std::map<Request<T>*, double>& coeffs)
      : coeffs(coeffs) {}

  inline virtual double convert(
      boost::shared_ptr<T> offer,
      Arc const* a,
      ExchangeTranslationContext<T> const* ctx) const {
    return offer->quantity() * coeffs.at(ctx->node_to_request.at(a->unode()));
  }

  virtual bool operator==(Converter<T>& other) const {
    QtyCoeffConverter<T>* cast = dynamic_cast<QtyCoeffConverter<T>*>(&other);
    return cast != NULL && coeffs == cast->coeffs;
  }

  std::map<Request<T>*, double> coeffs;
};

/// @class RequestPortfolio
///
/// @brief A RequestPortfolio is a group of (possibly constrained) requests for
/// resources
///
/// The portfolio contains a grouping of resource requests that may be mutually
/// met by suppliers. These requests may share a common set of
/// constraints. Take, for instance, a facility that needs fuel, of which there
/// are
/// two commodity types, fuelA and fuelB. If some combination of the two suffice
/// the facility's needs, then requests for both would be added to the portfolio
/// along with a capacity constraint.
///
/// The ExchangeTranslator will automatically add a mass-based constraint, that
/// accounts for mutual requests, if the portfolio has them. , e.g.,
/// @code
///
/// RequestPortfolio<SomeResource>::Ptr rp(new
/// RequestPortfolio<SomeResource>());
/// // add some requests
/// rp->AddRequest(/* args */);
/// // declare some of them as multi-commodity requests (i.e., any one will
/// // satisfy this demand).
/// rp->AddMutualReqs(/* args */);
///
/// @endcode
///
/// A default constraint will add unity for normal requests in the portfolio,
/// but will add a weighted coefficient for requests that meet the same mutual
/// demand. For example, if 10 kg of MOX and 9 kg of UOX meet the same demand
/// for fuel, coefficients are added such that a full order of either will
/// determine the demand as "met". In this case, the total demand is 9.5, the
/// MOX order is given a coefficient of 9.5 / 10, and the UOX order is given a
/// coefficient of 9.5 / 9.
template <class T>
class RequestPortfolio
    : public boost::enable_shared_from_this<RequestPortfolio<T>> {
 public:
  typedef boost::shared_ptr<RequestPortfolio<T>> Ptr;
  typedef std::function<double(boost::shared_ptr<T>)> cost_function_t;
  typedef Request<T>* request_ptr;

  RequestPortfolio() : requester_(NULL), qty_(0) {}

  /// deletes all requests associated with it
  ~RequestPortfolio() {
    typename std::vector<Request<T>*>::iterator it;
    for (it = requests_.begin(); it != requests_.end(); ++it) {
      delete *it;
    }
  }

  /// @brief add a request to the portfolio
  /// @param target the target resource associated with this request
  /// @param requester the requester
  /// @param commodity the commodity associated with this request
  /// @param preference the preference associated with this request (relative to
  /// others in the portfolio)
  /// @param exclusive a flag denoting that this request must be met
  /// exclusively,
  /// i.e., in its entirety by a single offer
  /// @param cost_function The cost function that the requester sets so that the
  /// bidder may evaluate many potential resources.
  /// @throws KeyError if a request is added from a different requester than the
  /// original or if the request quantity is different than the original
  Request<T>* AddRequest(boost::shared_ptr<T> target, Trader* requester,
                         std::string commodity, double preference,
                         bool exclusive, cost_function_t cost_function) {
    Request<T>* r =
        Request<T>::Create(target, requester, this->shared_from_this(),
                           commodity, preference, exclusive, cost_function);
    VerifyRequester_(r);
    requests_.push_back(r);
    mass_coeffs_[r] = 1;
    qty_ += target->quantity();
    return r;
  }
  /// @brief add a request to the portfolio
  /// @param target the target resource associated with this request
  /// @param requester the requester
  /// @param commodity the commodity associated with this request
  /// @param preference the preference associated with this request (relative to
  /// others in the portfolio)
  /// @param exclusive a flag denoting that this request must be met
  /// exclusively,
  /// i.e., in its entirety by a single offer
  /// @throws KeyError if a request is added from a different requester than the
  /// original or if the request quantity is different than the original
  Request<T>* AddRequest(boost::shared_ptr<T> target, Trader* requester,
                         std::string commodity = "",
                         double preference = kDefaultPref,
                         bool exclusive = false) {
    return AddRequest(target, requester, commodity, preference, exclusive,
                      NULL);
  }

  /// @brief adds a collection of requests (already having been registered with
  /// this portfolio) as multicommodity requests
  /// @param rs the collection of requests to add
  inline void AddMutualReqs(const std::vector<Request<T>*>& rs) {
    double avg_qty =
        std::accumulate(rs.begin(), rs.end(), 0.0, SumQty<T>) / rs.size();
    double qty;
    Request<T>* r;
    for (int i = 0; i < rs.size(); i++) {
      r = rs[i];
      qty = r->target()->quantity();
      mass_coeffs_[r] = r->target()->quantity() / avg_qty;
      qty_ -= qty;
    }
    qty_ += avg_qty;
  }

  /// @brief add a capacity constraint associated with the portfolio, if it
  /// doesn't already exist
  /// @param c the constraint to add
  inline void AddConstraint(const CapacityConstraint<T>& c) {
    constraints_.insert(c);
  }

  /// @return the agent associated with the portfolio. if no reqeusts have
  /// been added, the requester is NULL.
  inline Trader* requester() const { return requester_; }

  /// @return the request quantity associated with the portfolio.
  ///
  /// @brief, all requests in a portfolio must have the same quantity, which is
  /// checked during AddRequest()
  inline double qty() const { return qty_; }

  /// @return const access to the unconstrained requests
  inline const std::vector<Request<T>*>& requests() const { return requests_; }

  /// @return const access to the request constraints
  inline const std::set<CapacityConstraint<T>>& constraints() const {
    return constraints_;
  }

  /// returns a capacity converter for this portfolios request quantities
  inline typename Converter<T>::Ptr qty_converter() {
    return typename Converter<T>::Ptr(new QtyCoeffConverter<T>(mass_coeffs_));
  }

 private:
  /// @brief copy constructor is private to prevent copying and preserve
  /// explicit single-ownership of requests
  RequestPortfolio(const RequestPortfolio& rhs) {
    requester_ = rhs.requester_;
    requests_ = rhs.requests_;
    constraints_ = rhs.constraints_;
    qty_ = rhs.qty_;
    typename std::vector<Request<T>*>::iterator it;
    for (it = requests_.begin(); it != requests_.end(); ++it) {
      it->get()->set_portfolio(this->shared_from_this());
    }
  }

  /// @brief if the requester has not been determined yet, it is set. otherwise
  /// VerifyRequester() verifies the the request is associated with the
  /// portfolio's
  /// requester
  /// @throws KeyError if a request is added from a different requester than the
  /// original
  void VerifyRequester_(const Request<T>* r) {
    if (requester_ == NULL) {
      requester_ = r->requester();
    } else if (requester_ != r->requester()) {
      std::string msg = "Insertion error: requesters do not match.";
      throw KeyError(msg);
    }
  }

  /// requests_ is a vector because many requests may be identical, i.e., a set
  /// is not appropriate
  std::vector<Request<T>*> requests_;

  /// coefficients for the default mass constraint for known resources
  std::map<Request<T>*, double> mass_coeffs_;

  /// constraints_ is a set because constraints are assumed to be unique
  std::set<CapacityConstraint<T>> constraints_;

  /// the total quantity of resources associated with the portfolio
  double qty_;
  Trader* requester_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_REQUEST_PORTFOLIO_H_
