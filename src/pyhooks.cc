#include "Python.h"
#include "pyhooks.h"

#ifdef CYCLUS_WITH_PYTHON
#include <stdlib.h>

extern "C" {
#include "eventhooks.h"
#include "pyinfile.h"
#include "pymodule.h"
}

namespace cyclus {
int PY_INTERP_COUNT = 0;
bool PY_INTERP_INIT = false;


void PyAppendInitTab(void) {
  if (PyImport_AppendInittab("eventhooks", PyInit_eventhooks) == -1) {
    fprintf(stderr, "Error appending 'eventhooks' to the initialization table\n");
  }

  if (PyImport_AppendInittab("pyinfile", PyInit_pyinfile) == -1) {
    fprintf(stderr, "Error appending 'pyinfile' to the initialization table\n");
  }

  if (PyImport_AppendInittab("pymodule", PyInit_pymodule) == -1) {
    fprintf(stderr, "Error appending 'pymodule' to the initialization table\n");
  }
}

void PyImportInit(void) {
  PyObject* module_eventhooks = PyImport_ImportModule("eventhooks");
  if (module_eventhooks == NULL) {
    PyErr_Print(); // Print Python error information
    fprintf(stderr, "Error importing 'eventhooks' module\n");
  }

  PyObject* module_pyinfile = PyImport_ImportModule("pyinfile");
  if (module_pyinfile == NULL) {
    PyErr_Print();
    fprintf(stderr, "Error importing 'pyinfile' module\n");
  }

  PyObject* module_pymodule = PyImport_ImportModule("pymodule");
  if (module_pymodule == NULL) {
    PyErr_Print();
    fprintf(stderr, "Error importing 'pymodule' module\n");
  }
}

void PyImportCallInit(void) {
  PyObject* init_eventhooks = PyInit_eventhooks();
  if (init_eventhooks == NULL) {
    PyErr_Print();
    fprintf(stderr, "Error calling PyInit_eventhooks()\n");
  }

  PyObject* init_pyinfile = PyInit_pyinfile();
  if (init_pyinfile == NULL) {
    PyErr_Print();
    fprintf(stderr, "Error calling PyInit_pyinfile()\n");
  }

  PyObject* init_pymodule = PyInit_pymodule();
  if (init_pymodule == NULL) {
    PyErr_Print();
    fprintf(stderr, "Error calling PyInit_pymodule()\n");
  }
}


void PyStart(void) {
  if (!PY_INTERP_INIT) {
    PyAppendInitTab();
    Py_Initialize();
    PyImportInit();
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

std::string PyFindModule(std::string lib) { return CyclusPyFindModule(lib); };

Agent* MakePyAgent(std::string lib, std::string agent, void* ctx) {
  return CyclusMakePyAgent(lib, agent, ctx);
};

void InitFromPyAgent(Agent* src, Agent* dst, void* ctx) {
  CyclusInitFromPyAgent(src, dst, ctx);
};

void ClearPyAgentRefs(void) { CyclusClearPyAgentRefs(); };

void PyDelAgent(int i) { CyclusPyDelAgent(i); };

namespace toolkit {
std::string PyToJson(std::string infile) { return CyclusPyToJson(infile); };

std::string JsonToPy(std::string infile) { return CyclusJsonToPy(infile); };

void PyCallListeners(std::string tstype, Agent* agent, void* cpp_ctx, int time, boost::spirit::hold_any value){
    CyclusPyCallListeners(tstype, agent, cpp_ctx, time, value);
};

}  // namespace toolkit
}  // namespace cyclus
#else   // else CYCLUS_WITH_PYTHON
#include "error.h"

namespace cyclus {
int PY_INTERP_COUNT = 0;
bool PY_INTERP_INIT = false;

void PyAppendInitTab(void) {};

void PyImportInit(void) {};

void PyImportCallInit(void) {};

void PyStart(void) {};

void PyStop(void) {};

void EventLoop(void) {};

std::string PyFindModule(std::string lib) { return std::string(""); };

Agent* MakePyAgent(std::string lib, std::string agent, void* ctx) { return NULL; };

void InitFromPyAgent(Agent* src, Agent* dst, void* ctx) {};

void ClearPyAgentRefs(void) {};

void PyDelAgent(int i) {};

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

void PyCallListeners(std::string tsname, Agent* agent, void* cpp_ctx, int time, boost::spirit::hold_any value) {};

} // namespace toolkit
} // namespace cyclus
#endif  // ends CYCLUS_WITH_PYTHON
