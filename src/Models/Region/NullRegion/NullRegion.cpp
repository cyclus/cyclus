// NullRegion.cpp
// Implements the NullRegion class
#include <iostream>
#include "Logger.h"

#include "NullRegion.h"




/* --------------------
 *  * all MODEL classes have these members
 *   * --------------------
 *    */

extern "C" Model* constructNullRegion() {
      return new NullRegion();
}

extern "C" void destructNullRegion(Model* p) {
      delete p;
}

/* -------------------- */
