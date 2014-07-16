// Copyright (C) 2000, International Business Machines
// Corporation and others.  All Rights Reserved.
// This code is licensed under the terms of the Eclipse Public License (EPL).

#ifndef CYCLUS_SRC_COIN_HELPERS_H_
#define CYCLUS_SRC_COIN_HELPERS_H_

#include "OsiSolverInterface.hpp"

// This file holds helper functions for the coin-related libraries. They are
// generally small helpers included in the most current trunk, but not the most
// stable release. Some small changes have been made to the original file. See
// http://list.coin-or.org/pipermail/cbc/2014-February/001241.html for the
// original impetus.

namespace cyclus {

//  Check two agents against each other.  Return nonzero if different.
//  Ignore names if that set.
//  (Note initial version does not check names)
//  May modify both agents by cleaning up
int differentAgent(OsiSolverInterface & lhs, OsiSolverInterface & rhs,
                   bool ignoreNames = true);

}  // namespace cyclus

#endif  // CYCLUS_SRC_COIN_HELPERS_H_
