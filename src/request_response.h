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
  /// @return the request being responded to
  cyclus::Request<T>* request;

  /// @return the response object for the request
  boost::shared_ptr<T> response;

  /// @return the model responding the request
  cyclus::FacilityModel* responder;
};

} // namespace cyclus

#endif
