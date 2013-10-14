#ifndef CYCLUS_REQUEST_RESPONSE_CONSTRAINT_H_
#define CYCLUS_REQUEST_RESPONSE_CONSTRAINT_H_

#include <boost/shared_ptr.hpp>

#include "facility_model.h"
#include "request.h"

namespace cyclus {

/// A CapacityConstraint provides an ability to determine an agent's
/// constraints on resource allocation given a capacity.
template <class T>
struct CapacityConstraint {
  /// @brief the constraints capacity
  double capacity;
  
  /// @brief the capacity conversion function, which takes the resource type as
  /// an argument and converts it into the capacity type
  double (*converter)(boost::shared_ptr<T>); 
};

} // namespace cyclus

#endif
