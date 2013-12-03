#ifndef CYCLUS_CAPACITY_CONSTRAINT_H_
#define CYCLUS_CAPACITY_CONSTRAINT_H_

#include <boost/shared_ptr.hpp>

namespace cyclus {

/// @class Converter
///
/// @brief a simple interface for converting resource objects to unit capacities
template<class T>
struct Converter {
  typedef boost::shared_ptr< Converter<T> > Ptr;

  Converter() {}
  virtual ~Converter() {}
  
  virtual double convert(boost::shared_ptr<T>) = 0;
};

/// @class TrivialConverter
///
/// @brief The default converter: just return 1
template<class T>
struct TrivialConverter : public Converter<T> {
  TrivialConverter() {}
  virtual ~TrivialConverter() {}
  
  virtual double convert(boost::shared_ptr<T>) { return 1; }
};

/// @class CapacityConstraint
///
/// @brief A CapacityConstraint provides an ability to determine an agent's
/// constraints on resource allocation given a capacity.
template <class T>
class CapacityConstraint {
 public:
  /// @brief constructor for a constraint with a non-trivial converter
  CapacityConstraint(double capacity, typename Converter<T>::Ptr converter)
    : capacity_(capacity),
      converter_(converter),
      id_(next_id_++) {};

  /// @brief constructor for a constraint with a trivial converter (i.e., one
  /// that simply returns 1)
  explicit CapacityConstraint(double capacity)
    : capacity_(capacity),
      id_(next_id_++) {
    converter_ = typename Converter<T>::Ptr(new TrivialConverter<T>());
  }
  
  /// @return the constraints capacity
  inline double capacity() const {
    return capacity_;
  }

  /// @return the converter
  inline typename Converter<T>::Ptr converter() const {
    return converter_;
  }

  inline double convert(boost::shared_ptr<T> item) const {
    return converter_->convert(item);
  }

  /// @return a unique id for the constraint
  inline int id() const {
    return id_;
  }

 private:
  double capacity_;
  typename Converter<T>::Ptr converter_;
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
