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


#ifdef __APPLE__
// for some reason this symbol doesn't exist in the mac binaries
#include <stdexcept>
void OsiSolverInterface::addCol(CoinPackedVectorBase const& vec, double collb,
                                double colub, double obj, std::string name) {
  throw std::runtime_error("OsiSolverInterface::addCol() not implemented");
}
#endif