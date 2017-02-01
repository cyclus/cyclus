#ifndef CYCLUS_SRC_PYHOOKS_H_
#define CYCLUS_SRC_PYHOOKS_H_

#include <string>

namespace cyclus {
class Agent;

/// Because of NumPy #7595, we can only initialize & finalize the Python
/// interpreter once. This variable keeps a count of how many times we have
/// initialized so that we can know when to really stop the interpreter.
/// When Python binding are not installed, this will always remain zero.
extern int PY_INTERP_COUNT;

/// Whether or not the Python interpreter has been initilized.
extern bool PY_INTERP_INIT;

/// Convience function for initializing Python hooks
void PyInitHooks(void);

/// Initialize Python functionality, this is a no-op if Python was not
/// installed along with Cyclus. This may be called many times and safely
/// initializes the Python interpreter only once.
void PyStart(void);

/// Closes the current Python session. This is a no-op if Python was
/// not installed with Cyclus. This may safely be called many times.
void PyStop(void);

// Add some simple shims that attach C++ to Python C hooks
void EventLoop(void);

/// Finds a Python module and returns its filename.
std::string PyFindModule(std::string);

/// Finds a Python module and returns an agent pointer from it.
Agent* MakePyAgent(std::string, std::string, void*);

/// Removes all Python agents from the internal cache. There is usually
/// no need for a user to call this.
void ClearPyAgentRefs(void);

/// Removes a single Python agent from the reference cache.
void PyDelAgent(int);

namespace toolkit {
/// Convert Python simulation string to JSON
std::string PyToJson(std::string);

/// Convert JSON string to Python simulation string
std::string JsonToPy(std::string);

}  // ends namespace toolkit
}  // ends namespace cyclus
#endif  // ends CYCLUS_SRC_PYHOOKS_H_
