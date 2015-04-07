#ifndef CYCLUS_SRC_REQUEST_H_
#define CYCLUS_SRC_REQUEST_H_

#include <ostream>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace cyclus {

class Trader;
template <class T> class RequestPortfolio;

/// @class Request
///
/// @brief A Request encapsulates all the information required to communicate
/// the needs of an agent in the Dynamic Resource Exchange, including the
/// commodity it needs as well as a resource specification for that commodity.
/// A Request is templated its resource.
template <class T>
class Request {
 public:
  /// @brief a factory method for a request
  /// @param target the target resource associated with this request
  /// @param requester the requester
  /// @param portfolio the porftolio of which this request is a part
  /// @param commodity the commodity associated with this request
  /// @param preference the preference associated with this request (relative to
  /// others in the portfolio)
  /// @param exclusive a flag denoting that this request must be met exclusively,
  /// i.e., in its entirety by a single offer
  inline static Request<T>* Create(
      boost::shared_ptr<T> target,
      Trader* requester,
      typename RequestPortfolio<T>::Ptr portfolio,
      std::string commodity = "",
      double preference = 1,
      bool exclusive = false) {
    return new Request<T>(target, requester, portfolio,
                          commodity, preference, exclusive);
  }

  /// @brief a factory method for a bid for a bid without a portfolio
  /// @warning this factory should generally only be used for testing
  inline static Request<T>* Create(boost::shared_ptr<T> target,
                                   Trader* requester,
                                   std::string commodity = "",
                                   double preference = 1,
                                   bool exclusive = false) {
    return new Request<T>(target, requester, commodity, preference,
                          exclusive);
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

 private:
  /// @brief constructors are private to require use of factory methods
  Request(boost::shared_ptr<T> target, Trader* requester,
          std::string commodity = "", double preference = 1,
          bool exclusive = false)
      : target_(target),
        requester_(requester),
        commodity_(commodity),
        preference_(preference),
        exclusive_(exclusive) {}

  Request(boost::shared_ptr<T> target, Trader* requester,
          typename RequestPortfolio<T>::Ptr portfolio,
          std::string commodity = "", double preference = 1,
          bool exclusive = false)
      : target_(target),
        requester_(requester),
        commodity_(commodity),
        preference_(preference),
        portfolio_(portfolio),
        exclusive_(exclusive) {}

  boost::shared_ptr<T> target_;
  Trader* requester_;
  double preference_;
  std::string commodity_;
  boost::weak_ptr<RequestPortfolio<T> > portfolio_;
  bool exclusive_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_REQUEST_H_
