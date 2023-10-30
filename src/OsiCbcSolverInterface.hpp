#ifndef OsiCbcSolverInterface_HPP
#define OsiCbcSolverInterface_HPP

#include "CbcConfig.h"

// headers in this file below this pragma have all warnings shushed
#pragma GCC system_header

// CBC_VERSION_MAJOR defined for Cbc > 2.5
#ifndef CBC_VERSION_MAJOR
#error "Cyclus cannot yet handle your version of CoinCBC. Please open an issue with your CoinCBC version."
#elif CBC_VERSION_MAJOR == 2 && CBC_VERSION_MINOR < 8
#include "OsiCbcSolverInterface_2_5.hpp"
#elif CBC_VERSION_MAJOR == 2 && CBC_VERSION_MINOR < 10
#include "OsiCbcSolverInterface_2_8.hpp"
#else
#include "OsiCbcSolverInterface_2_10.hpp"
#endif

#endif
