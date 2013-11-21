#ifndef CYCLUS_CAPACITY_CONSTRAINT_H_
#define CYCLUS_CAPACITY_CONSTRAINT_H_

#include <boost/shared_ptr.hpp>

namespace cyclus {

/// @class CapacityConstraint
///
/// @brief A CapacityConstraint provides an ability to determine an agent's
/// constraints on resource allocation given a capacity.
template <class T>
struct CapacityConstraint {
 public:
  /// constructor
  CapacityConstraint() : id_(next_id_++) { };
  
  /// @brief the constraints capacity
  double capacity;
  
  /// @brief the capacity conversion function, which takes the resource type as
  /// an argument and converts it into the capacity type
  double (*converter)(boost::shared_ptr<T>);

  /// @return a unique id for the constraint
  int id() const {return id_;};

  /* -------------------- private methods and members ----------------------- */  
  int id_;
  static int next_id_;
};

template<class T> int CapacityConstraint<T>::next_id_ = 0;

/// @brief equality operator
template<class T>
bool operator==(const CapacityConstraint<T>& lhs,
                const CapacityConstraint<T>& rhs) {
  return  ((lhs.capacity == rhs.capacity) &&
           (lhs.converter == rhs.converter));
};

/// @brief comparison operator, allows usage in ordered containers
template<class T>
bool operator<(const CapacityConstraint<T>& lhs,
               const CapacityConstraint<T>& rhs) {
  return  (lhs.id() < rhs.id());
};

} // namespace cyclus

#endif
