// Copyright (C) 2000, International Business Machines
// Corporation and others.  All Rights Reserved.
// This code is licensed under the terms of the Eclipse Public License (EPL).

#include "coin_helpers.h"

#include "CoinPackedMatrix.hpp"
#include "CoinFloatEqual.hpp"

/// taken directly from OsiSolverInterface.cpp on 2/17/14 from
/// https://projects.coin-or.org/Osi/browser/trunk. Thanks to John Forrest for
/// his assistance! Some small changes have been made to the original file.

namespace cyclus {

/* Check two models against each other.  Return nonzero if different.
   Ignore names if that set.
   May modify both models by cleaning up
*/
int 
differentModel(OsiSolverInterface & lhs, OsiSolverInterface & rhs,
               bool /*ignoreNames*/)
{
  // set reasonable defaults
  bool takeHint;
  OsiHintStrength strength;
  // Switch off printing if asked to
  bool gotHint = (lhs.getHintParam(OsiDoReducePrint,takeHint,strength));
  assert (gotHint);
  bool printStuff=true;
  // if (strength!=OsiHintIgnore&&takeHint) // always printStuff
  //   printStuff=false;
  int returnCode=0;
  int numberRows = lhs.getNumRows();
  int numberColumns = lhs.getNumCols();
  int numberIntegers = lhs.getNumIntegers();
  if (numberRows!=rhs.getNumRows()||numberColumns!=rhs.getNumCols()) {
    if (printStuff)
      printf("** Mismatch on size, this has %d rows, %d columns - rhs has %d rows, %d columns\n",
             numberRows,numberColumns,rhs.getNumRows(),rhs.getNumCols());
    return 1000;
  }
  if (numberIntegers!=rhs.getNumIntegers()) {
    if (printStuff)
      printf("** Mismatch on number of integers, this has %d - rhs has %d\n",
             numberIntegers,rhs.getNumIntegers());
    return 1001;
  }
  int numberErrors1=0;
  int numberErrors2=0;
  for (int i=0;i<numberColumns;i++) {
    if (lhs.isInteger(i)) {
      if (!rhs.isInteger(i))
	numberErrors1++;
    } else {
      if (rhs.isInteger(i))
	numberErrors2++;
    }
  }
  if (numberErrors1||numberErrors2) {
    if (printStuff)
      printf("** Mismatch on integers, %d (this int, rhs not), %d (this not rhs int)\n",
             numberErrors1,numberErrors2);
    return 1002;
  }
  // Arrays
  const double * rowLower = lhs.getRowLower();
  const double * rowUpper = lhs.getRowUpper();
  const double * columnLower = lhs.getColLower();
  const double * columnUpper = lhs.getColUpper();
  const double * objective = lhs.getObjCoefficients();
  const double * rowLower2 = rhs.getRowLower();
  const double * rowUpper2 = rhs.getRowUpper();
  const double * columnLower2 = rhs.getColLower();
  const double * columnUpper2 = rhs.getColUpper();
  const double * objective2 = rhs.getObjCoefficients();
  const CoinPackedMatrix * matrix = lhs.getMatrixByCol();
  const CoinPackedMatrix * matrix2 = rhs.getMatrixByCol();
  CoinRelFltEq tolerance;
  int numberDifferentL = 0;
  int numberDifferentU = 0;
  for (int i=0;i<numberRows;i++) {
    if (!tolerance(rowLower[i],rowLower2[i]))
      numberDifferentL++;
    if (!tolerance(rowUpper[i],rowUpper2[i]))
      numberDifferentU++;
  }
  int n = numberDifferentL+numberDifferentU;
  returnCode+=n;
  if (n&&printStuff)
    printf("Row differences , %d lower, %d upper\n",
	   numberDifferentL,numberDifferentU);
  numberDifferentL = 0;
  numberDifferentU = 0;
  int numberDifferentO = 0;
  for (int i=0;i<numberColumns;i++) {
    if (!tolerance(columnLower[i],columnLower2[i]))
      numberDifferentL++;
    if (!tolerance(columnUpper[i],columnUpper2[i]))
      numberDifferentU++;
    if (!tolerance(objective[i],objective2[i]))
      numberDifferentO++;
  }
  n = numberDifferentL+numberDifferentU+numberDifferentO;
  returnCode+=n;
  if (n&&printStuff)
    printf("Column differences , %d lower, %d upper, %d objective\n",
	   numberDifferentL,numberDifferentU,numberDifferentO);
  if (matrix->getNumElements()==rhs.getNumElements()) {
    if (!matrix->isEquivalent(*matrix2,tolerance)) {
      returnCode+=100;
      if (printStuff)
	printf("Two matrices are not same\n");
    }
  } else {
    returnCode+=200;
    if (printStuff)
      printf("Two matrices are not same - %d elements and %d elements\n",
	     matrix->getNumElements(),matrix2->getNumElements());
  }
  return returnCode;
}

} // namespace cyclus
