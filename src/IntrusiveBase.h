
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
    if (((const IntrusiveBase*) p)->counter_ == 0) {
      MLOG(LEV_DEBUG4) << "Object " << p << " created";
    }
    ++((const IntrusiveBase*) p)->counter_;
    
    MLOG(LEV_DEBUG5) << "Object " << p << " ref_count=" << ((const IntrusiveBase*) p)->counter_;
  }

  friend void intrusive_ptr_release(const Derived* p) {
    BOOST_ASSERT(p);
    MLOG(LEV_DEBUG5) << "Object " << p << " ref_count=" << ((const IntrusiveBase*) p)->counter_ - 1;
    if (--((const IntrusiveBase*) p)->counter_ == 0) {
      MLOG(LEV_DEBUG4) << "Object " << p << " deleted";
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

