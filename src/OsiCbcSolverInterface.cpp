// #include "CbcConfig.h"
#include "OsiCbcSolverInterface.hpp"

// CBC_VERSION_MAJOR defined for Cbc > 2.5
// #ifndef CBC_VERSION_MAJOR
// #include "OsiCbcSolverInterface_2_5.cpp"
// #elif CBC_VERSION_MAJOR == 2 && CBC_VERSION_MINOR <= 8
// #include "OsiCbcSolverInterface_2_8.cpp"
// #elif CBC_VERSION_MAJOR == 2 && CBC_VERSION_MINOR <= 10
// #include "OsiCbcSolverInterface_2_10.cpp"
// #else
// #error "Cyclus cannot yet handle your version of CoinCBC. Please open an issue with your CoinCBC version."
// #endif


// for some reason these symbol doesn't exist in the mac binaries
// Those were also not present in condaforge/linux-anvil-comp7 docker container
// used to build conda recipes, using cpp/cxx 7.3, it also works with the docker
// in unit test...
void OsiSolverInterface::addCol(CoinPackedVectorBase const& vec, double collb,
                                double colub, double obj, std::string name) {
  // just ignore the name
  addCol(vec, collb, colub, obj);
}

void OsiSolverInterface::addCol(int numberElements, const int* rows,
                                const double* elements, double collb, double colub,
                                double obj, std::string name) {
  // just ignore the name
  addCol(numberElements, rows, elements, collb, colub, obj);
}

void OsiSolverInterface::addRow(CoinPackedVectorBase const& vec, char rowsen,
                                double rowrhs, double rowrng, std::string name) {
  // just ignore the name
  addRow(vec, rowsen, rowrhs, rowrng, name);
}

void OsiSolverInterface::addRow(CoinPackedVectorBase const& vec, double rowlb,
                                double rowub, std::string name) {
  // just ignore the name
  addRow(vec, rowlb, rowub);
}
