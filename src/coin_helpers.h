#ifndef CYCLUS_SRC_COIN_HELPERS_H_
#define CYCLUS_SRC_COIN_HELPERS_H_

#include "OsiSolverInterface.hpp"

/// This file holds helper functions for the coin-related libraries. They are
/// generally small helpers included in the most current trunk, but not the most
/// stable release. See
/// http://list.coin-or.org/pipermail/cbc/2014-February/001241.html for one such
/// example's impetus.

namespace cyclus {

/** Check two models against each other.  Return nonzero if different.
    Ignore names if that set.
    (Note initial version does not check names)
    May modify both models by cleaning up
*/
int differentModel(OsiSolverInterface & lhs, OsiSolverInterface & rhs,
                   bool ignoreNames=true);
  
} // namespace cyclus

#endif // CYCLUS_SRC_COIN_HELPERS_H_
