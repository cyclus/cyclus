#ifndef CYCLUS_SRC_INTRUSIVE_BASE_H_
#define CYCLUS_SRC_INTRUSIVE_BASE_H_

#include <boost/intrusive_ptr.hpp>
#include <boost/assert.hpp>

#include "logger.h"

namespace cyclus {

/// IntrusiveBase provides a base class that fulfulls basic requirements
/// for a (sub) class to be used in a boost::intrusive_ptr.
///
/// Allows subclasses to track their reference count via the onboard
/// "counter_" member variable. To use a class as a boost::intrusive_ptr,
/// it should inherit from IntrusiveBase with default access (NOT
/// public). All destructors of subclasses (and subsub, etc.) should be
/// virtual to ensure memory deallocation occurs properly for objects
/// that have been up-casted:
/// @code
/// class Resource: IntrusiveBase<Resource> {
/// ...
/// virtual ~Resource();
/// ...
/// }
///
/// int main(...) {
/// boost::intrusive_ptr<Resource> Resource(new Resource());
/// boost::intrusive_ptr<Resource> resource2(new Resource());
///
/// // use resource as if it were a regular pointer, e.g.
/// double quantity = resource->quantity();
///
/// // equals operator compares raw pointer values:
///
/// // always true
/// if (resource == resource) {}
///
/// // always false
/// if (resource == resource2) {}
///
/// // don't worry about deallocation - it will be automatic.
/// }
/// @endcode
template <class Derived> class IntrusiveBase {
  /// used by boost::intrusive_ptr to increase object's reference count
  friend void intrusive_ptr_add_ref(const Derived* p) {
    BOOST_ASSERT(p);
    ++((const IntrusiveBase*) p)->counter_;
  }

  /// used by boost::intrusive_ptr to decrease object's reference count
  /// and deallocate the object if the ref count is zero.
  friend void intrusive_ptr_release(const Derived* p) {
    BOOST_ASSERT(p);
    if (--((const IntrusiveBase*) p)->counter_ == 0) {
      delete p;
    }
  }

 protected:
  /// protected because we don't want direct instantiations of
  IntrusiveBase(): counter_(0) {}

  ~IntrusiveBase() {}

  /// the copy constructor must zero out the ref count
  IntrusiveBase(const IntrusiveBase&) : counter_(0) {}

  IntrusiveBase& operator=(const IntrusiveBase&) {
    return *this;
  }

 private:
  /// tracks an object's reference count
  mutable unsigned long counter_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_INTRUSIVE_BASE_H_
