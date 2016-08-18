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


// for some reason these symbol doesn't exist in the mac binaries
void OsiSolverInterface::addCol(CoinPackedVectorBase const& vec, double collb,
                                double colub, double obj, std::string name) {
  // just ignore the name
  addCol(vec, collb, colub, obj);
}

void addCol(int numberElements, const int* rows, const double* elements,
            const double collb, const double colub,
            const double obj, std::string name) {
  // just ignore the name
  //addCol(numberElements, rows, elements, collb, colub, obj);
}
#ifdef __APPLE__
#endif