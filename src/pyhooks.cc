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

void CallCythonHelper(std::string module_name, PyObject* spec = NULL, PyObject* spec_globals = NULL, PyObject* mod = NULL) {
    PyObject *maybe_mod = PyInit_pymodule();
    if (!maybe_mod) goto pyerror;
    if (Py_IS_TYPE(maybe_mod, &PyModuleDef_Type)) {
        spec_globals = PyDict_New();
        if (!spec_globals) goto pyerror;
        std::string py_snippet = 
            "import importlib.machinery as im\n"
            "spec = im.ModuleSpec('" + module_name + "', None)\n";
        PyObject *res = PyRun_String(py_snippet.c_str(), Py_file_input, spec_globals, spec_globals);
        Py_XDECREF(res); // don't use res whether or not it's set
        if (!res) goto pyerror;
        spec = PyDict_GetItemString(spec_globals, "spec");
        if (!spec) goto pyerror;               
        
        mod = PyModule_FromDefAndSpec((PyModuleDef*) maybe_mod, spec);
        if (!mod) goto pyerror;
        int execRes = PyModule_ExecDef(mod, (PyModuleDef*) maybe_mod);
        if (execRes) goto pyerror;
    } else {
        mod = maybe_mod;
    }

    if (false) {
      pyerror:
      PyErr_Print();
    }
}

template<typename T, typename... Args> 
T CallCythonNonVoid(std::string module_name, std::function<T(Args...)> module_method, Args... args) {
  PyObject *spec = NULL, *spec_globals = NULL, *mod = NULL;
  CallCythonHelper(module_name, spec, spec_globals, mod);
  T result = module_method(args...);
  Py_XDECREF(mod);
  Py_XDECREF(spec);
  Py_XDECREF(spec_globals);
  return result;
  
}

template<typename... Args>
void CallCythonVoid(std::string module_name, std::function<void(Args...)> module_method, Args... args) {
  PyObject *spec = NULL, *spec_globals = NULL, *mod = NULL;
  CallCythonHelper(module_name, spec, spec_globals, mod);
  module_method(args...);
  Py_XDECREF(mod);
  Py_XDECREF(spec);
  Py_XDECREF(spec_globals);
}


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

void EventLoop(void) { 
  std::function<void()> module_method = CyclusEventLoopHook;
  CallCythonVoid("evenhooks", module_method);
}

std::string PyFindModule(std::string lib) { 
  std::function<std::string(std::string)> module_method = CyclusPyFindModule;
  return CallCythonNonVoid("pymodule", module_method, lib); 
}

Agent* MakePyAgent(std::string lib, std::string agent, void* ctx) {
  std::function<cyclus::Agent*(std::string, std::string, void*)> module_method = CyclusMakePyAgent;
  return CallCythonNonVoid("pymodule", module_method, lib, agent, ctx); 
};

void InitFromPyAgent(Agent* src, Agent* dst, void* ctx) {
  std::function<void(cyclus::Agent*, cyclus::Agent*, void*)> module_method = CyclusInitFromPyAgent;
  CallCythonVoid("pymodule", module_method, src, dst, ctx);
};

void ClearPyAgentRefs(void) { 
  std::function<void()> module_method = CyclusClearPyAgentRefs;
  CallCythonVoid("pymodule", module_method);
};


void PyDelAgent(int i) { 
  std::function<void(int)> module_method = CyclusPyDelAgent;
  CallCythonVoid("pymodule", module_method, i);
};

namespace toolkit {
std::string PyToJson(std::string infile) { 
  std::function<std::string(std::string)> module_method = CyclusPyToJson;
  return CallCythonNonVoid("pyinfile", module_method, infile);
};

std::string JsonToPy(std::string infile) { 
  std::function<std::string(std::string)> module_method = CyclusJsonToPy;
  return CallCythonNonVoid("pyinfile", module_method, infile);
};

void PyCallListeners(std::string tstype, Agent* agent, void* cpp_ctx, int time, boost::spirit::hold_any value){
    std::function<void(std::string, cyclus::Agent*, void*, int, boost::spirit::hold_any)> module_method = CyclusPyCallListeners;
    CallCythonVoid("pymodule", module_method, tstype, agent, cpp_ctx, time, value);
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
