
// InstrusiveBase.h

#ifndef INTRUSIVE_BASE_H
#define INTRUSIVE_BASE_H

#include <boost/intrusive_ptr.hpp>
#include <boost/assert.hpp>
#include "Logger.h"

template <class Derived>
class IntrusiveBase {
  friend void intrusive_ptr_add_ref(const Derived* p) {
    BOOST_ASSERT(p);
    ++((const IntrusiveBase*) p)->counter_;
  }

  friend void intrusive_ptr_release(const Derived* p) {
    BOOST_ASSERT(p);
    if (--((const IntrusiveBase*) p)->counter_ == 0) {
      delete p;
    }
  }

  protected:
    IntrusiveBase(): counter_(0) {}
    ~IntrusiveBase() { }

    IntrusiveBase(const IntrusiveBase&) : counter_(0) {}
    IntrusiveBase& operator=(const IntrusiveBase&) { return *this; }

  private:
    mutable unsigned long counter_;
};

#endif

