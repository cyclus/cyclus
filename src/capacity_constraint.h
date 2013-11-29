#ifndef CYCLUS_CAPACITY_CONSTRAINT_H_
#define CYCLUS_CAPACITY_CONSTRAINT_H_

#include <boost/shared_ptr.hpp>

namespace cyclus {

/// @class CapacityConstraint
///
/// @brief A CapacityConstraint provides an ability to determine an agent's
/// constraints on resource allocation given a capacity.
template <class T>
class CapacityConstraint {
 public:
  typedef double (*Converter)(T*);
  
  /// constructor
  CapacityConstraint(double capacity, Converter converter)
    : capacity_(capacity),
      converter_(converter),
      id_(next_id_++) {};
  
  /// @return the constraints capacity
  inline double capacity() const {
    return capacity_;
  }

  /// @return the converter
  inline Converter converter() const {
    return converter_;
  }

  /// @return the converted value of a given item
  inline double convert(T* item) const {
    return (*converter_)(item);
  }
  inline double convert(boost::shared_ptr<T> item) const {
    return convert(item.get());
  }

  /// @return a unique id for the constraint
  inline int id() const {
    return id_;
  }

  /* -------------------- private methods and members ----------------------- */  
  double capacity_;
  Converter converter_;
  /// double (*converter_)(T*);
  int id_;
  static int next_id_;
};

template<class T> int CapacityConstraint<T>::next_id_ = 0;

/// @brief CapacityConstraint-CapacityConstraint equality operator
template<class T>
inline bool operator==(const CapacityConstraint<T>& lhs,
                       const CapacityConstraint<T>& rhs) {
  return  ((lhs.capacity() == rhs.capacity()) &&
           (lhs.converter() == rhs.converter()));
};

/// @brief CapacityConstraint-CapacityConstraint comparison operator, allows
/// usage in ordered containers
template<class T>
inline bool operator<(const CapacityConstraint<T>& lhs,
                      const CapacityConstraint<T>& rhs) {
  return  (lhs.id() < rhs.id());
};

} // namespace cyclus

#endif
