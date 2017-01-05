#ifndef CYCLUS_SRC_PYHOOKS_H_
#define CYCLUS_SRC_PYHOOKS_H_

namespace cyclus {
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
/// installed along with Cyclus.
void PyStart(void);

/// Closes the current Python session. This is a no-op if Python was
/// not installed with Cyclus.
void PyStop(void);

// Add some simple shims that attach C++ to Python C hooks
void EventLoop(void);
}
#endif  // ends CYCLUS_SRC_PYHOOKS_H_
