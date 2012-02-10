// NullRegion.cpp
// Implements the NullRegion class
#include <iostream>
#include "Logger.h"

#include "NullRegion.h"

/* --------------------
   output database info
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string NullRegion::outputDir_ = "/null";


/* --------------------
  all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructNullRegion() {
      return new NullRegion();
}


/* -------------------- */
