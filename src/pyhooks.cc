#include "pyhooks.h"

#ifdef CYCLUS_WITH_PYTHON
#include <stdlib.h>

#include "Python.h"

extern "C" {
#include "eventhooks.h"
}

namespace cyclus {
int PY_INTERP_COUNT = 0;
bool PY_INTERP_INIT = false;

void PyInitHooks(void) {
#if PY_MAJOR_VERSION < 3
  initeventhooks();
#else
  PyInit_eventhooks();
#endif
};

void PyStart(void) {
  if (!PY_INTERP_INIT) {
    Py_Initialize();
    PyInitHooks();
    atexit(PyStop);
    PY_INTERP_INIT = true;
  };
  PY_INTERP_COUNT++;
};

void PyStop(void) {
  PY_INTERP_COUNT--;

  // PY_INTERP_COUNT should only be negative when called atexit()
  if (PY_INTERP_INIT && PY_INTERP_COUNT < 0) {
    Py_Finalize();
  };
};

void EventLoop(void) { CyclusEventLoopHook(); };
}  // namespace cyclus
#else   // else CYCLUS_WITH_PYTHON
namespace cyclus {
int PY_INTERP_COUNT = 0;
bool PY_INTERP_INIT = false;

void PyInitHooks(void) {};

void PyStart(void) {};

void PyStop(void) {};

void EventLoop(void) {};
} // namespace cyclus
#endif  // ends CYCLUS_WITH_PYTHON
