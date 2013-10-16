#ifndef CYCLUS_BID_H_
#define CYCLUS_BID_H_

#include <boost/shared_ptr.hpp>

#include "facility_model.h"
#include "request.h"

namespace cyclus {

/// A Bid encapsulates all the information required to
/// communicate a bid to a request for a resource, including the
/// resource bid and the responder.
template <class T>
struct Bid {
  /// constructor
  Bid() : id_(next_id_++) { };
  
  /// @return the request being responded to
  cyclus::Request<T>* request;

  /// @return the bid object for the request
  boost::shared_ptr<T> bid;

  /// @return the model responding the request
  cyclus::FacilityModel* responder;
  
  /// @return a unique id for the bid
  const int id() const {return id_;};

 private:
  int id_;
  static int next_id_;
};

template<class T> int Bid<T>::next_id_ = 0;

/// @brief equality operator
template<class T>
bool operator==(const cyclus::Bid<T>& lhs,
                const cyclus::Bid<T>& rhs) {
  return  ((lhs.request == rhs.request) &&
           (lhs.bid == rhs.bid) &&
           (lhs.responder == rhs.responder));
}

/// @brief comparison operator, allows usage in ordered containers
template<class T>
bool operator<(const cyclus::Bid<T>& lhs,
               const cyclus::Bid<T>& rhs) {
  return  (lhs.id() < rhs.id());
};

} // namespace cyclus

#endif
