#ifndef CYCLUS_REQUEST_RESPONSE_H_
#define CYCLUS_REQUEST_RESPONSE_H_

#include <boost/shared_ptr.hpp>

#include "facility_model.h"
#include "request.h"

namespace cyclus {

/// A RequestResponse encapsulates all the information required to
/// communicate a response to a request for a resource, including the
/// resource response and the responder.
template <class T>
struct RequestResponse {
  /// constructor
  RequestResponse() : id_(next_id_++) { };
  
  /// @return the request being responded to
  cyclus::Request<T>* request;

  /// @return the response object for the request
  boost::shared_ptr<T> response;

  /// @return the model responding the request
  cyclus::FacilityModel* responder;
  
  /// @return a unique id for the response
  const int id() const {return id_;};

 private:
  int id_;
  static int next_id_;
};

template<class T> int RequestResponse<T>::next_id_ = 0;

/// @brief equality operator
template<class T>
bool operator==(const cyclus::RequestResponse<T>& lhs,
                const cyclus::RequestResponse<T>& rhs) {
  return  ((lhs.request == rhs.request) &&
           (lhs.response == rhs.response) &&
           (lhs.responder == rhs.responder));
}

/// @brief comparison operator, allows usage in ordered containers
template<class T>
bool operator<(const cyclus::RequestResponse<T>& lhs,
               const cyclus::RequestResponse<T>& rhs) {
  return  (lhs.id() < rhs.id());
};

} // namespace cyclus

#endif
