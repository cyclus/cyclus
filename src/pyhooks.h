#ifndef CYCLUS_SRC_PYHOOKS_H_
#define CYCLUS_SRC_PYHOOKS_H_

#ifdef CYCLUS_WITH_PYTHON
#include "Python.h"

extern "C" {
#include "eventhooks.h"
}

namespace cyclus {
  // Add some simple shims that attach C++ to Python C hooks
  void EventLoop(void) { __pyx_f_10eventhooks_eventloophook(); };
}
#endif  // ends CYCLUS_WITH_PYTHON
#endif  // ends CYCLUS_SRC_PYHOOKS_H_
