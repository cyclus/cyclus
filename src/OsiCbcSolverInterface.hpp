#ifndef OsiCbcSolverInterface_HPP
#define OsiCbcSolverInterface_HPP

//#include <cstring>
#include <stdio.h>
#include <string.h>
#include "version.h"

// #define VERSION_COMPARE(A, B) ( A[0] >= B[0] && A[2] >= B[2] && A[4] >=B[4] )

// #if VERSION_COMPARE(CBC_VERSION, "2.6.0")
#ifdef CBC_VERSION
//#define CBC_VERSION_COMP 
//#if CBC_VERSION_COMP
  #if strcmp(CBC_VERSION, "2.5.0")
    #include "OsiCbcSolverInterface_2_8.hpp"
  #else
    #include "OsiCbcSolverInterface_2_5.hpp"
  #endif
#endif

#endif
