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

  /// Initialize Python functionality, this is a no-op if Python was not
  /// installed along with Cyclus.
  inline void PyStart(void) {
    Py_Initialize();
    PyInitHooks();
  };

  /// Closes the current Python session. This is a no-op if Python was
  /// not installed with Cyclus.
  inline void PyStop(void) { Py_Finalize(); };

  // Add some simple shims that attach C++ to Python C hooks
  inline void EventLoop(void) { CyclusEventLoopHook(); };
}
#else   // else CYCLUS_WITH_PYTHON
namespace cyclus {
  /// Initialize Python functionality, this is a no-op if Python was not
  /// installed along with Cyclus.
  inline void PyStart(void) {};

  /// Closes the current Python session. This is a no-op if Python was
  /// not installed with Cyclus.
  inline void PyStop(void) {};
}
#endif  // ends CYCLUS_WITH_PYTHON
#endif  // ends CYCLUS_SRC_PYHOOKS_H_
