#include "Python.h"
#include "pyhooks.h"

#ifdef CYCLUS_WITH_PYTHON
#include <stdlib.h>

#include "error.h"
#include "eventhooks_api.h"
#include "pyinfile_api.h"
#include "pymodule_api.h"

namespace cyclus {
namespace {
// The import_<module>() functions that Cython generates return -1 with a
// Python exception set when the import fails, and leave the pointers to the
// functions of the module null, so calling one of them would be a
// segmentation fault. As of Python 3.13 the import also fails if a Python
// exception was already set, which is the case after a Python agent raises an
// exception, as nothing checks for one after calling into an agent.
void CheckImport(int status, std::string module) {
  if (status < 0) {
    PyErr_Print();
    throw Error("Failed to import the Cyclus Python module '" + module +
                "', see the Python traceback above.");
  }
}
}  // namespace

int PY_INTERP_COUNT = 0;
bool PY_INTERP_INIT = false;

void PyStart(void) {
  if (!PY_INTERP_INIT) {
    Py_Initialize();
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

void EventLoop(void) {
  CheckImport(import_eventhooks(), "eventhooks");
  eventloophook();
};

std::string PyFindModule(std::string lib) {
  CheckImport(import_pymodule(), "pymodule");
  return py_find_module(lib);
};

Agent* MakePyAgent(std::string lib, std::string agent, void* ctx) {
  CheckImport(import_pymodule(), "pymodule");
  return make_py_agent(lib, agent, ctx);
};

void InitFromPyAgent(Agent* src, Agent* dst, void* ctx) {
  CheckImport(import_pymodule(), "pymodule");
  init_from_py_agent(src, dst, ctx);
};

void ClearPyAgentRefs(void) {
  CheckImport(import_pymodule(), "pymodule");
  clear_pyagent_refs();
};

void PyDelAgent(int i) {
  CheckImport(import_pymodule(), "pymodule");
  py_del_agent(i);
};

namespace toolkit {
std::string PyToJson(std::string infile) {
  CheckImport(import_pyinfile(), "pyinfile");
  return py_to_json(infile);
};

std::string JsonToPy(std::string infile) {
  CheckImport(import_pyinfile(), "pyinfile");
  return json_to_py(infile);
};

void PyCallListeners(std::string tstype, Agent* agent, void* cpp_ctx, int time,
                     boost::spirit::hold_any value) {
  CheckImport(import_pymodule(), "pymodule");
  py_call_listeners(tstype, agent, cpp_ctx, time, value);
};

}  // namespace toolkit
}  // namespace cyclus
#else  // else CYCLUS_WITH_PYTHON
#include "error.h"

namespace cyclus {
int PY_INTERP_COUNT = 0;
bool PY_INTERP_INIT = false;

void PyStart(void) {};

void PyStop(void) {};

void EventLoop(void) {};

std::string PyFindModule(std::string lib) {
  return std::string("");
};

Agent* MakePyAgent(std::string lib, std::string agent, void* ctx) {
  return NULL;
};

void InitFromPyAgent(Agent* src, Agent* dst, void* ctx) {};

void ClearPyAgentRefs(void) {};

void PyDelAgent(int i) {};

namespace toolkit {
std::string PyToJson(std::string infile) {
  throw cyclus::ValidationError(
      "Cannot convert from Python input files since "
      "Cyclus was not built with Python bindings.");
  return "";
};

std::string JsonToPy(std::string infile) {
  throw cyclus::ValidationError(
      "Cannot convert to Python input files since "
      "Cyclus was not built with Python bindings.");
  return "";
};

void PyCallListeners(std::string tsname,
                     Agent* agent,
                     void* cpp_ctx,
                     int time,
                     boost::spirit::hold_any value) {};

}  // namespace toolkit
}  // namespace cyclus
#endif  // ends CYCLUS_WITH_PYTHON
