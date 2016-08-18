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

void OsiSolverInterface::addCol(int numberElements, const int* rows,
                                const double* elements, double collb, double colub,
                                double obj, std::string name) {
  // just ignore the name
  //addCol(numberElements, rows, elements, collb, colub, obj);
}

/*
OsiSolverInterface::addCol(int, int const*, double const*, double, double, double, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >)", referenced from:
      vtable for OsiCbcSolverInterface in OsiCbcSolverInterface.cpp.o
  "OsiSolverInterface::addRow(CoinPackedVectorBase const&, char, double, double, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >)", referenced from:
      vtable for OsiCbcSolverInterface in OsiCbcSolverInterface.cpp.o
  "OsiSolverInterface::addRow(CoinPackedVectorBase const&, double, double, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >)
*/

#ifdef __APPLE__
#endif