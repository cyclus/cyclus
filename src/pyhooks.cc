#include "pyhooks.h"

#ifdef CYCLUS_WITH_PYTHON
#include <stdlib.h>

#include "Python.h"

extern "C" {
#include "eventhooks.h"
#include "pyinfile.h"
}

namespace cyclus {
int PY_INTERP_COUNT = 0;
bool PY_INTERP_INIT = false;

void PyInitHooks(void) {
#if PY_MAJOR_VERSION < 3
  initeventhooks();
  initpyinfile();
#else
  PyInit_eventhooks();
  PyInit_pyinfile();
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

namespace toolkit {
std::string PyToJson(std::string infile) { return CyclusPyToJson(infile); };

std::string JsonToPy(std::string infile) { return CyclusJsonToPy(infile); };
}  // namespace toolkit
}  // namespace cyclus
#else   // else CYCLUS_WITH_PYTHON
namespace cyclus {
int PY_INTERP_COUNT = 0;
bool PY_INTERP_INIT = false;

void PyInitHooks(void) {};

void PyStart(void) {};

void PyStop(void) {};

void EventLoop(void) {};

namespace toolkit {
std::string PyToJson(std::string infile) {
  throw cyclus::ValidationError("Cannot convert from Python input files since "
                                "Cyclus was not built with Python bindings.");
  return "";
};

std::string JsonToPy(std::string infile) {
  throw cyclus::ValidationError("Cannot convert to Python input files since "
                                "Cyclus was not built with Python bindings.");
  return "";
};
} // namespace toolkit
} // namespace cyclus
#endif  // ends CYCLUS_WITH_PYTHON
