#ifndef CYCLUS_SRC_BID_H_
#define CYCLUS_SRC_BID_H_

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "request.h"
#include "package.h"

namespace cyclus {

/// Default unit_cost values are zero
static const double kDefaultUnitCost = 0.0;

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
  /// @param portfolio the portfolio of which this bid is a part
  /// @param exclusive flag for whether the bid is exclusive
  /// @param unit_cost the unit_cost associated with providing the resource 
  ///                  from the request. 
  inline static Bid<T>* Create(Request<T>* request,
                               boost::shared_ptr<T> offer,
                               Trader* bidder,
                               typename BidPortfolio<T>::Ptr portfolio,
                               bool exclusive,
                               double unit_cost,
                               Package::Ptr package = Package::unpackaged()) {
    return new Bid<T>(request, offer, bidder, portfolio, exclusive, 
                      unit_cost, package);
  }

  /// @brief a factory method for a bid
  /// @param request the request being responded to by this bid
  /// @param offer the resource being offered in response to the request
  /// @param bidder the bidder
  /// @param portfolio the porftolio of which this bid is a part
  /// @param exclusive flag for whether the bid is exclusive
  inline static Bid<T>* Create(Request<T>* request,
                               boost::shared_ptr<T> offer,
                               Trader* bidder,
                               typename BidPortfolio<T>::Ptr portfolio,
                               bool exclusive = false,
                               Package::Ptr package = Package::unpackaged()) {
    return Create(request, offer, bidder, portfolio, exclusive,
                  kDefaultUnitCost, package);
  }
  /// @brief a factory method for a bid without a portfolio
  /// @warning this factory should generally only be used for testing
  inline static Bid<T>* Create(Request<T>* request, boost::shared_ptr<T> offer,
                               Trader* bidder, bool exclusive,
                               double unit_cost,
                               Package::Ptr package = Package::unpackaged()) {
    return new Bid<T>(request, offer, bidder, exclusive, unit_cost, package);
  }
  /// @brief a factory method for a bid for a bid without a portfolio
  /// @warning this factory should generally only be used for testing
  inline static Bid<T>* Create(Request<T>* request, boost::shared_ptr<T> offer,
                               Trader* bidder, bool exclusive = false,
                               Package::Ptr package = Package::unpackaged()) {
    return Create(request, offer, bidder, exclusive,
                  kDefaultUnitCost, package);
  }

  /// @return the request being responded to
  inline Request<T>* request() const { return request_; }

  /// @return the bid object for the request
  inline boost::shared_ptr<T> offer() const { return offer_; }

  /// @return the agent responding to the request
  inline Trader* bidder() const { return bidder_; }

  /// @return the portfolio of which this bid is a part
  inline typename BidPortfolio<T>::Ptr portfolio() { return portfolio_.lock(); }

  /// @return whether or not this an exclusive bid
  inline bool exclusive() const { return exclusive_; }

  /// @return the unit_cost of this bid
  inline double unit_cost() const { return unit_cost_; }

  /// @brief Set the unit_cost of this bid
  /// @param unit_cost 
  inline void unit_cost(double unit_cost) {unit_cost_ = unit_cost;}

 private:
  /// @brief constructors are private to require use of factory methods
  Bid(Request<T>* request, boost::shared_ptr<T> offer, Trader* bidder,
      bool exclusive, double unit_cost,
      Package::Ptr package = Package::unpackaged())
      : request_(request),
        offer_(offer),
        bidder_(bidder),
        exclusive_(exclusive),
        unit_cost_(unit_cost),
        package_(package) {}
  /// @brief constructors are private to require use of factory methods
  Bid(Request<T>* request, boost::shared_ptr<T> offer, Trader* bidder,
      bool exclusive = false, Package::Ptr package = Package::unpackaged())
      : request_(request),
        offer_(offer),
        bidder_(bidder),
        exclusive_(exclusive),
        unit_cost_(kDefaultUnitCost),
        package_(package) {}

  Bid(Request<T>* request, boost::shared_ptr<T> offer, Trader* bidder,
      typename BidPortfolio<T>::Ptr portfolio, bool exclusive,
      double unit_cost, Package::Ptr package = Package::unpackaged())
      : request_(request),
        offer_(offer),
        bidder_(bidder),
        portfolio_(portfolio),
        exclusive_(exclusive),
        unit_cost_(unit_cost),
        package_(package) {}

  Bid(Request<T>* request, boost::shared_ptr<T> offer, Trader* bidder,
      typename BidPortfolio<T>::Ptr portfolio, bool exclusive = false,
      Package::Ptr package = Package::unpackaged())
      : request_(request),
        offer_(offer),
        bidder_(bidder),
        portfolio_(portfolio),
        exclusive_(exclusive),
        unit_cost_(kDefaultUnitCost),
        package_(package) {}

  Request<T>* request_;
  boost::shared_ptr<T> offer_;
  Trader* bidder_;
  boost::weak_ptr<BidPortfolio<T>> portfolio_;
  bool exclusive_;
  double unit_cost_;
  Package::Ptr package_;
};

}  // namespace cyclus
#endif  // CYCLUS_SRC_BID_H_
