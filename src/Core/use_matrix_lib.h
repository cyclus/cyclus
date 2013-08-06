// UseMatLib.h contains all of the declarations needed to use a specific
// Matrix and/or Vector library in place of the default librares for the
// decay calculations performed on a Material object.

#ifndef USEMATLIB_H
#define USEMATLIB_H

// To change the matrix library used:
//
//      #include "<Matrix Library>"
#include "l_matrix.h"          

namespace cyclus {

// To change the matrix type: 
//
//      typedef <Matrix Type> Matrix;
typedef LMatrix Matrix;

// To change the vector type:
//
//      typedef <Vector Type> Vector;
typedef LMatrix Vector;
} // namespace cyclus
#endif
