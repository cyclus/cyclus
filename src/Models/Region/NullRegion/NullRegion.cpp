// NullRegion.cpp
// Implements the NullRegion class

#include <iostream>

#include "NullRegion.h"

#include "Logger.h"

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

std::string NullRegion::str() {
  return RegionModel::str();
};

extern "C" Model* constructNullRegion() {
      return new NullRegion();
}

/* -------------------- */
