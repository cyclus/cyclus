#ifndef CYCLUS_SRC_BID_H_
#define CYCLUS_SRC_BID_H_

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <limits>

#include "request.h"
#include "package.h"

namespace cyclus {

class Trader;
template <class T> class BidPortfolio;

/// @class Bid
///
/// @brief A Bid encapsulates all the information required to communicate a bid
/// response to a request for a resource, including the resource bid and the
/// bidder.
template <class T> class Bid {
 public:
  /// @brief a factory method for a bid
  /// @param request the request being responded to by this bid
  /// @param offer the resource being offered in response to the request
  /// @param bidder the bidder
  /// @param portfolio the porftolio of which this bid is a part
  /// @param exclusive flag for whether the bid is exclusive
  /// @param preference specifies the preference of a bid in a request
  ///        to bid arc. If NaN the request preference is used.
  ///        WARNING: This should only be set by the bidder using the
  ///        requests callback cost function. Bidders should not
  ///        arbitrarily set this preference.
  inline static Bid<T>* Create(Request<T>* request,
                               boost::shared_ptr<T>
                                   offer,
                               Trader* bidder,
                               typename BidPortfolio<T>::Ptr portfolio,
                               bool exclusive,
                               double preference) {
    return new Bid<T>(request, offer, bidder, portfolio, exclusive, preference);
  }

  /// @brief a factory method for a bid
  /// @param request the request being responded to by this bid
  /// @param offer the resource being offered in response to the request
  /// @param bidder the bidder
  /// @param portfolio the porftolio of which this bid is a part
  /// @param exclusive flag for whether the bid is exclusive
  inline static Bid<T>* Create(Request<T>* request,
                               boost::shared_ptr<T>
                                   offer,
                               Trader* bidder,
                               typename BidPortfolio<T>::Ptr portfolio,
                               bool exclusive = false,
                               Package::Ptr package = Package::unpackaged()) {
    return Create(request, offer, bidder, portfolio, exclusive,
                  std::numeric_limits<double>::quiet_NaN(), package);
  }
  /// @brief a factory method for a bid for a bid without a portfolio
  /// @warning this factory should generally only be used for testing
  inline static Bid<T>* Create(Request<T>* request, boost::shared_ptr<T> offer,
                               Trader* bidder, bool exclusive,
                               double preference,
                               Package::Ptr package = Package::unpackaged()) {
    return new Bid<T>(request, offer, bidder, exclusive, preference, package);
  }
  /// @brief a factory method for a bid for a bid without a portfolio
  /// @warning this factory should generally only be used for testing
  inline static Bid<T>* Create(Request<T>* request, boost::shared_ptr<T> offer,
                               Trader* bidder, bool exclusive = false,
                               Package::Ptr package = Package::unpackaged()) {
    return Create(request, offer, bidder, exclusive,
                  std::numeric_limits<double>::quiet_NaN(), package);
  }

  /// @return the request being responded to
  inline Request<T>* request() const { return request_; }

  /// @return the bid object for the request
  inline boost::shared_ptr<T> offer() const { return offer_; }

  /// @return the agent responding the request
  inline Trader* bidder() const { return bidder_; }

  /// @return the portfolio of which this bid is a part
  inline typename BidPortfolio<T>::Ptr portfolio() { return portfolio_.lock(); }

  /// @return whether or not this an exclusive bid
  inline bool exclusive() const { return exclusive_; }

  /// @return the preference of this bid
  inline double preference() const { return preference_; }

 private:
  /// @brief constructors are private to require use of factory methods
  Bid(Request<T>* request, boost::shared_ptr<T> offer, Trader* bidder,
      bool exclusive, double preference,
      Package::Ptr package = Package::unpackaged())
      : request_(request),
        offer_(offer),
        bidder_(bidder),
        exclusive_(exclusive),
        preference_(preference),
        package_(package) {}
  /// @brief constructors are private to require use of factory methods
  Bid(Request<T>* request, boost::shared_ptr<T> offer, Trader* bidder,
      bool exclusive = false, Package::Ptr package = Package::unpackaged())
      : request_(request),
        offer_(offer),
        bidder_(bidder),
        exclusive_(exclusive),
        preference_(std::numeric_limits<double>::quiet_NaN()),
        package_(package) {}

  Bid(Request<T>* request, boost::shared_ptr<T> offer, Trader* bidder,
      typename BidPortfolio<T>::Ptr portfolio, bool exclusive, double preference,
      Package::Ptr package = Package::unpackaged())
      : request_(request),
        offer_(offer),
        bidder_(bidder),
        portfolio_(portfolio),
        exclusive_(exclusive),
        preference_(preference),
        package_(package) {}

  Bid(Request<T>* request, boost::shared_ptr<T> offer, Trader* bidder,
      typename BidPortfolio<T>::Ptr portfolio, bool exclusive = false,
      Package::Ptr package = Package::unpackaged())
      : request_(request),
        offer_(offer),
        bidder_(bidder),
        portfolio_(portfolio),
        exclusive_(exclusive),
        preference_(std::numeric_limits<double>::quiet_NaN()),
        package_(package)  {}

  Request<T>* request_;
  boost::shared_ptr<T> offer_;
  Trader* bidder_;
  boost::weak_ptr<BidPortfolio<T>> portfolio_;
  bool exclusive_;
  double preference_;
  Package::Ptr package_;
};

}  // namespace cyclus
#endif  // CYCLUS_SRC_BID_H_
