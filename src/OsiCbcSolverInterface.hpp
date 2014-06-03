#ifndef OsiCbcSolverInterface_HPP
#define OsiCbcSolverInterface_HPP

#include "CbcConfig.h"

// headers in this file below this pragma have all warnings shushed
#pragma GCC system_header

// CBC_VERSION_MAJOR defined for Cbc > 2.5
#ifndef CBC_VERSION_MAJOR
#include "OsiCbcSolverInterface_2_5.hpp"
#else
#include "OsiCbcSolverInterface_2_8.hpp"
#endif

#endif
