#ifndef CYCLUS_SRC_PYHOOKS_H_
#define CYCLUS_SRC_PYHOOKS_H_

#ifdef CYCLUS_WITH_PYTHON
#include "Python.h"

extern "C" {
#include "eventhooks.h"
}

namespace cyclus {
  /// Convience function for initializing Python hooks
  inline void PyInitHooks(void) {
  #if PY_MAJOR_VERSION < 3
    initeventhooks();
  #else
    PyInit_eventhooks();
  #endif
  };

  // Add some simple shims that attach C++ to Python C hooks
  inline void EventLoop(void) { __pyx_f_10eventhooks_eventloophook(); };
}
#endif  // ends CYCLUS_WITH_PYTHON
#endif  // ends CYCLUS_SRC_PYHOOKS_H_
