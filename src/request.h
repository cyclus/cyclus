#ifndef REQUEST_H_
#define REQUEST_H_

#include <string>

#include "material.h"
#include "facility_model.h"


namespace cyclus {
  
/// A Request encapsulates all the information required to communicate the
/// needs of an agent in the Dynamic Resource Exchange, including the
/// commodity it needs as well as a resource specification for that commodity.
/// A Request is templated its resource.
template <class T>
struct Request {
  /// @return the commodity associated with this request
  std::string commodity;

  /// @return the target resource for this request
  T* target;

  /// @return the preference value for this request
  double preference;

  /// @return the model requesting the resource
  cyclus::FacilityModel* requester;
};

} // namespace cyclus

#endif
