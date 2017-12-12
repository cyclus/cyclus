#ifndef CYCLUS_SRC_REQUEST_H_
#define CYCLUS_SRC_REQUEST_H_

#include <functional>
#include <ostream>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace cyclus {

class Material;

/// Default preference values are unity. This has been updated from values of
/// zero (which was the case prior to release 1.4). Preferences can be lower or
/// higher than the default value, but must be positive.
static const double kDefaultPref = 1;

class Trader;
template <class T> class RequestPortfolio;

/// @class Request
///
/// @brief A Request encapsulates all the information required to communicate
/// the needs of an agent in the Dynamic Resource Exchange, including the
/// commodity it needs as well as a resource specification for that commodity.
/// A Request is templated its resource.
template <class T> class Request {
 public:
  typedef std::function<double(boost::shared_ptr<T>)> cost_function_t;

  /// @brief a factory method for a request
  /// @param target the target resource associated with this request
  /// @param requester the requester
  /// @param portfolio the porftolio of which this request is a part
  /// @param commodity the commodity associated with this request
  /// @param preference the preference associated with this request (relative to
  /// others in the portfolio)
  /// @param exclusive a flag denoting that this request must be met
  /// exclusively,
  /// i.e., in its entirety by a single offer
  /// @param cost_function a standard function object that returns the cost of a
  /// potential resource when called.
  inline static Request<T>* Create(boost::shared_ptr<T> target,
                                   Trader* requester,
                                   typename RequestPortfolio<T>::Ptr portfolio,
                                   std::string commodity,
                                   double preference,
                                   bool exclusive,
                                   cost_function_t cost_function) {
    return new Request<T>(target, requester, portfolio, commodity, preference,
                          exclusive, cost_function);
  }
  /// @brief a factory method for a request
  /// @param target the target resource associated with this request
  /// @param requester the requester
  /// @param portfolio the porftolio of which this request is a part
  /// @param commodity the commodity associated with this request
  /// @param preference the preference associated with this request (relative to
  /// others in the portfolio)
  /// @param exclusive a flag denoting that this request must be met
  /// exclusively,
  /// i.e., in its entirety by a single offer
  inline static Request<T>* Create(boost::shared_ptr<T> target,
                                   Trader* requester,
                                   typename RequestPortfolio<T>::Ptr portfolio,
                                   std::string commodity = "",
                                   double preference = kDefaultPref,
                                   bool exclusive = false) {
    return Create(target, requester, portfolio, commodity, preference,
                  exclusive, NULL);
  }

  /// @brief a factory method for a bid for a bid without a portfolio
  /// @warning this factory should generally only be used for testing
  inline static Request<T>* Create(boost::shared_ptr<T> target,
                                   Trader* requester,
                                   std::string commodity,
                                   double preference,
                                   bool exclusive,
                                   cost_function_t cost_function) {
    return new Request<T>(target, requester, commodity, preference, exclusive,
                          cost_function);
  }
  /// @brief a factory method for a bid for a bid without a portfolio
  /// @warning this factory should generally only be used for testing
  inline static Request<T>* Create(boost::shared_ptr<T> target,
                                   Trader* requester,
                                   std::string commodity = "",
                                   double preference = kDefaultPref,
                                   bool exclusive = false) {
    return Create(target, requester, commodity, preference, exclusive, NULL);
  }

  /// @return this request's target
  inline boost::shared_ptr<T> target() const { return target_; }

  /// @return the requester associated with this request
  inline Trader* requester() const { return requester_; }

  /// @return the commodity associated with this request
  inline std::string commodity() const { return commodity_; }

  /// @return the preference value for this request
  inline double preference() const { return preference_; }

  /// @return the portfolio of which this request is a part
  inline typename RequestPortfolio<T>::Ptr portfolio() const {
    return portfolio_.lock();
  }

  /// @return whether or not this an exclusive request
  inline bool exclusive() const { return exclusive_; }

  /// @return the cost function for the request
  inline cost_function_t cost_function() const { return cost_function_; }

 private:
  /// @brief constructors are private to require use of factory methods
  Request(boost::shared_ptr<T> target, Trader* requester, std::string commodity,
          double preference, bool exclusive, cost_function_t cost_function)
      : target_(target),
        requester_(requester),
        commodity_(commodity),
        preference_(preference),
        exclusive_(exclusive),
        cost_function_(cost_function) {}

  /// @brief constructors are private to require use of factory methods
  Request(boost::shared_ptr<T> target, Trader* requester,
          std::string commodity = "", double preference = kDefaultPref,
          bool exclusive = false)
      : target_(target),
        requester_(requester),
        commodity_(commodity),
        preference_(preference),
        exclusive_(exclusive),
        cost_function_(NULL) {}

  Request(boost::shared_ptr<T> target, Trader* requester,
          typename RequestPortfolio<T>::Ptr portfolio, std::string commodity,
          double preference, bool exclusive, cost_function_t cost_function)
      : target_(target),
        requester_(requester),
        commodity_(commodity),
        preference_(preference),
        portfolio_(portfolio),
        exclusive_(exclusive),
        cost_function_(cost_function) {}

  Request(boost::shared_ptr<T> target, Trader* requester,
          typename RequestPortfolio<T>::Ptr portfolio,
          std::string commodity = "", double preference = kDefaultPref,
          bool exclusive = false)
      : target_(target),
        requester_(requester),
        commodity_(commodity),
        preference_(preference),
        portfolio_(portfolio),
        exclusive_(exclusive),
        cost_function_(NULL) {}

  boost::shared_ptr<T> target_;
  Trader* requester_;
  double preference_;
  std::string commodity_;
  boost::weak_ptr<RequestPortfolio<T>> portfolio_;
  bool exclusive_;
  cost_function_t cost_function_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_REQUEST_H_
