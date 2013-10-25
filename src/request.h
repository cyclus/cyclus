#ifndef CYCLUS_REQUEST_H_
#define CYCLUS_REQUEST_H_

#include <string>
#include <ostream>
#include <boost/shared_ptr.hpp>

#include "cyc_limits.h"

namespace cyclus {

class Trader;
  
/// A Request encapsulates all the information required to communicate the
/// needs of an agent in the Dynamic Resource Exchange, including the
/// commodity it needs as well as a resource specification for that commodity.
/// A Request is templated its resource.
template <class T>
struct Request {
  /// @return the commodity associated with this request
  std::string commodity;

  /// @return the target resource for this request
  boost::shared_ptr<T> target;

  /// @return the preference value for this request
  double preference;

  /// @return the model requesting the resource
  Trader* requester;

};

template<class T>
bool operator==(const Request<T>& lhs, const Request<T>& rhs) {
  return  ((lhs.commodity == rhs.commodity) &&
           (lhs.target == rhs.target) &&
           (DoubleEq(lhs.preference, rhs.preference)) &&
           (lhs.requester == rhs.requester));
}

template<class T>
std::ostream& operator<<(std::ostream& os, const Request<T>& r) {
  os << r.commodity;
  return os;
};
} // namespace cyclus

#endif
