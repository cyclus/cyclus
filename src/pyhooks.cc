#include "Python.h"
#include "pyhooks.h"

#ifdef CYCLUS_WITH_PYTHON
#include <stdlib.h>

#include "eventhooks_api.h"
#include "pyinfile_api.h"
#include "pymodule_api.h"

namespace cyclus {
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
  import_eventhooks();
  eventloophook();
};

std::string PyFindModule(std::string lib) {
  import_pymodule();
  return py_find_module(lib);
};

Agent* MakePyAgent(std::string lib, std::string agent, void* ctx) {
  import_pymodule();
  return make_py_agent(lib, agent, ctx);
};

void InitFromPyAgent(Agent* src, Agent* dst, void* ctx) {
  import_pymodule();
  init_from_py_agent(src, dst, ctx);
};

void ClearPyAgentRefs(void) {
  import_pymodule();
  clear_pyagent_refs();
};

void PyDelAgent(int i) {
  import_pymodule();
  py_del_agent(i);
};

namespace toolkit {
std::string PyToJson(std::string infile) {
  import_pyinfile();
  return py_to_json(infile);
};

std::string JsonToPy(std::string infile) {
  import_pyinfile();
  return json_to_py(infile);
};

void PyCallListeners(std::string tstype, Agent* agent, void* cpp_ctx, int time,
                     boost::spirit::hold_any value) {
  import_pymodule();
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

void PyCallListeners(std::string tsname, Agent* agent, void* cpp_ctx, int time,
                     boost::spirit::hold_any value) {};

}  // namespace toolkit
}  // namespace cyclus
#endif  // ends CYCLUS_WITH_PYTHON
