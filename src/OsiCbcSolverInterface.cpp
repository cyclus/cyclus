// #ifndef OsiCbcSolverInterface_CPP
// #define OsiCbcSolverInterface_CPP

#include "OsiCbcSolverInterface.hpp"

#if CBC_VERSION_COMP
#include "OsiCbcSolverInterface_2_8.cpp"
#else
#include "OsiCbcSolverInterface_2_5.cpp"
#endif

// #endif
