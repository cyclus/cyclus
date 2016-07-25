#include "CbcConfig.h"

// CBC_VERSION_MAJOR defined for Cbc > 2.5
#ifndef CBC_VERSION_MAJOR
#include "OsiCbcSolverInterface_2_5.cpp"
#elif CBC_VERSION_MAJOR == 2 && CBC_VERSION_MINOR <= 8
#include "OsiCbcSolverInterface_2_8.cpp"
#elif CBC_VERSION_MAJOR == 2 && CBC_VERSION_MINOR == 9
#include "OsiCbcSolverInterface_2_9.cpp"
#else
#error "Cyclus cannot yet handle your version of CoinCBC. Please open an issue with your CoinCBC version."
#endif
