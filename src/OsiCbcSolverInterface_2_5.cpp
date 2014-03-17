// $Id: OsiCbcSolverInterface.cpp 1902 2013-04-10 16:58:16Z stefan $
// Copyright (C) 2002, International Business Machines
// Corporation and others.  All Rights Reserved.
// This code is licensed under the terms of the Eclipse Public License (EPL).

#include <cassert>

#include "OsiConfig.h"
#include "CoinTime.hpp"

#include "CoinHelperFunctions.hpp"
#include "CoinIndexedVector.hpp"

/*
  Default solver configuration: In any environment, clp is the default if you
  do nothing.
  
  The necessary definitions will be handled by the configure script (based on
  the value specified for --with-osicbc-default-solver) in any environment
  where configure is available. The results can be found in inc/config_osi.h,
  which is created during configuration from config_osi.h.in and placed
  in the build directory.

  In an environment which does not use configure (MS Visual Studio, for example)
  the preferred method is to edit the definitions in inc/OsiConfig.h. The
  symbols are left undefined by default because some older environments (e.g.,
  MS Visual Studio v7 or earlier) will not accept an #include directive which
  uses a macro to specify the file. In such an environment, if you want to use
  a solver other than OsiClp, you'll need to make the changes here.
*/

#ifndef OSICBC_DFLT_SOLVER
#define OSICBC_DFLT_SOLVER OsiClpSolverInterface
#define OSICBC_CLP_DFLT_SOLVER
#include "OsiClpSolverInterface.hpp"
#else
#include OSICBC_DFLT_SOLVER_HPP
#endif

#include "OsiCbcSolverInterface.hpp"
#include "OsiCuts.hpp"
#include "OsiRowCut.hpp"
#include "OsiColCut.hpp"
#ifdef OSICBC_CLP_DFLT_SOLVER
#include "ClpPresolve.hpp"
#endif
//#############################################################################
// Solve methods
//#############################################################################
void OsiCbcSolverInterface::initialSolve()
{
  agentPtr_->solver()->initialSolve();
}

//-----------------------------------------------------------------------------
void OsiCbcSolverInterface::resolve()
{
  agentPtr_->solver()->resolve();
}
//#############################################################################
// Parameter related methods
//#############################################################################

bool
OsiCbcSolverInterface::setIntParam(OsiIntParam key, int value)
{
  return agentPtr_->solver()->setIntParam(key,value);;
}

//-----------------------------------------------------------------------------

bool
OsiCbcSolverInterface::setDblParam(OsiDblParam key, double value)
{
  return agentPtr_->solver()->setDblParam(key,value);
}

//-----------------------------------------------------------------------------

bool
OsiCbcSolverInterface::setStrParam(OsiStrParam key, const std::string & value)
{
  return agentPtr_->solver()->setStrParam(key,value);
}


//-----------------------------------------------------------------------------

bool
OsiCbcSolverInterface::getIntParam(OsiIntParam key, int& value) const 
{
  return agentPtr_->solver()->getIntParam(key,value);
}

//-----------------------------------------------------------------------------

bool
OsiCbcSolverInterface::getDblParam(OsiDblParam key, double& value) const
{
  return agentPtr_->solver()->getDblParam(key,value);
}

//-----------------------------------------------------------------------------

bool
OsiCbcSolverInterface::getStrParam(OsiStrParam key, std::string & value) const
{
  if ( key==OsiSolverName ) {
    std::string value2;
    agentPtr_->solver()->getStrParam(key,value2);
    value = "cbc"+value2;
    return true;
  }
  return agentPtr_->solver()->getStrParam(key,value);
}


//#############################################################################
// Methods returning info on how the solution process terminated
//#############################################################################

bool OsiCbcSolverInterface::isAbandoned() const
{
  return agentPtr_->solver()->isAbandoned();
}

bool OsiCbcSolverInterface::isProvenOptimal() const
{
  return agentPtr_->solver()->isProvenOptimal();
}

bool OsiCbcSolverInterface::isProvenPrimalInfeasible() const
{
  return agentPtr_->solver()->isProvenPrimalInfeasible();
}

bool OsiCbcSolverInterface::isProvenDualInfeasible() const
{
  return agentPtr_->solver()->isProvenDualInfeasible();
}
bool OsiCbcSolverInterface::isPrimalObjectiveLimitReached() const
{
  return agentPtr_->solver()->isPrimalObjectiveLimitReached();
}

bool OsiCbcSolverInterface::isDualObjectiveLimitReached() const
{
  return agentPtr_->solver()->isDualObjectiveLimitReached();
}

bool OsiCbcSolverInterface::isIterationLimitReached() const
{
  return agentPtr_->solver()->isIterationLimitReached();
}

//#############################################################################
// WarmStart related methods
//#############################################################################
CoinWarmStart *OsiCbcSolverInterface::getEmptyWarmStart () const
{
  return agentPtr_->solver()->getEmptyWarmStart();
}

CoinWarmStart* OsiCbcSolverInterface::getWarmStart() const
{
  return agentPtr_->solver()->getWarmStart();
}

//-----------------------------------------------------------------------------

bool OsiCbcSolverInterface::setWarmStart(const CoinWarmStart* warmstart)
{
  return agentPtr_->solver()->setWarmStart(warmstart);
}

//#############################################################################
// Hotstart related methods (primarily used in strong branching)
//#############################################################################

void OsiCbcSolverInterface::markHotStart()
{
  agentPtr_->solver()->markHotStart();
}

void OsiCbcSolverInterface::solveFromHotStart()
{
  agentPtr_->solver()->solveFromHotStart();
}

void OsiCbcSolverInterface::unmarkHotStart()
{
  agentPtr_->solver()->unmarkHotStart();
}

//#############################################################################
// Problem information methods (original data)
//#############################################################################

//------------------------------------------------------------------
const char * OsiCbcSolverInterface::getRowSense() const
{
  return agentPtr_->solver()->getRowSense();
}
//------------------------------------------------------------------
const double * OsiCbcSolverInterface::getRightHandSide() const
{
  return agentPtr_->solver()->getRightHandSide();
}
//------------------------------------------------------------------
const double * OsiCbcSolverInterface::getRowRange() const
{
  return agentPtr_->solver()->getRowRange();
}
//------------------------------------------------------------------
// Return information on integrality
//------------------------------------------------------------------
bool OsiCbcSolverInterface::isContinuous(int colNumber) const
{
  return agentPtr_->solver()->isContinuous(colNumber);
}
//------------------------------------------------------------------

//------------------------------------------------------------------
// Row and column copies of the matrix ...
//------------------------------------------------------------------
const CoinPackedMatrix * OsiCbcSolverInterface::getMatrixByRow() const
{
  return agentPtr_->solver()->getMatrixByRow();
}

const CoinPackedMatrix * OsiCbcSolverInterface::getMatrixByCol() const
{
  return agentPtr_->solver()->getMatrixByCol();
}

//------------------------------------------------------------------
std::vector<double*> OsiCbcSolverInterface::getDualRays(int maxNumRays) const
{
  return agentPtr_->solver()->getDualRays(maxNumRays);
}
//------------------------------------------------------------------
std::vector<double*> OsiCbcSolverInterface::getPrimalRays(int maxNumRays) const
{
  return agentPtr_->solver()->getPrimalRays(maxNumRays);
}
//#############################################################################
void
OsiCbcSolverInterface::setContinuous(int index)
{
  agentPtr_->solver()->setContinuous(index);
}
//-----------------------------------------------------------------------------
void
OsiCbcSolverInterface::setInteger(int index)
{
  agentPtr_->solver()->setInteger(index);
}
//-----------------------------------------------------------------------------
void
OsiCbcSolverInterface::setContinuous(const int* indices, int len)
{
  agentPtr_->solver()->setContinuous(indices,len);
}
//-----------------------------------------------------------------------------
void
OsiCbcSolverInterface::setInteger(const int* indices, int len)
{
  agentPtr_->solver()->setInteger(indices,len);
}
//-----------------------------------------------------------------------------
void OsiCbcSolverInterface::setColSolution(const double * cs) 
{
  agentPtr_->solver()->setColSolution(cs);
}
//-----------------------------------------------------------------------------
void OsiCbcSolverInterface::setRowPrice(const double * rs) 
{
  agentPtr_->solver()->setRowPrice(rs);
}

//#############################################################################
// Problem modifying methods (matrix)
//#############################################################################
void 
OsiCbcSolverInterface::addCol(const CoinPackedVectorBase& vec,
			      const double collb, const double colub,   
			      const double obj)
{
  agentPtr_->solver()->addCol(vec,collb,colub,obj);
}
/* Add a column (primal variable) to the problem. */
void 
OsiCbcSolverInterface::addCol(int numberElements, const int * rows, const double * elements,
			   const double collb, const double colub,   
			   const double obj) 
{
  agentPtr_->solver()->addCol(numberElements, rows, elements,
                              collb,colub,obj);
}
//-----------------------------------------------------------------------------
void 
OsiCbcSolverInterface::addCols(const int numcols,
			       const CoinPackedVectorBase * const * cols,
			       const double* collb, const double* colub,   
			       const double* obj)
{
  agentPtr_->solver()->addCols(numcols,cols,collb,colub,obj);
}
//-----------------------------------------------------------------------------
void 
OsiCbcSolverInterface::deleteCols(const int num, const int * columnIndices)
{
  agentPtr_->solver()->deleteCols(num,columnIndices);
}
//-----------------------------------------------------------------------------
void 
OsiCbcSolverInterface::addRow(const CoinPackedVectorBase& vec,
			      const double rowlb, const double rowub)
{
  agentPtr_->solver()->addRow(vec,rowlb,rowub);
}
//-----------------------------------------------------------------------------
void 
OsiCbcSolverInterface::addRow(const CoinPackedVectorBase& vec,
			      const char rowsen, const double rowrhs,   
			      const double rowrng)
{
  agentPtr_->solver()->addRow(vec,rowsen,rowrhs,rowrng);
}
//-----------------------------------------------------------------------------
void 
OsiCbcSolverInterface::addRows(const int numrows,
			       const CoinPackedVectorBase * const * rows,
			       const double* rowlb, const double* rowub)
{
  agentPtr_->solver()->addRows(numrows,rows,rowlb,rowub);
}
//-----------------------------------------------------------------------------
void 
OsiCbcSolverInterface::addRows(const int numrows,
			       const CoinPackedVectorBase * const * rows,
			       const char* rowsen, const double* rowrhs,   
			       const double* rowrng)
{
  agentPtr_->solver()->addRows(numrows,rows,rowsen,rowrhs,rowrng);
}
//-----------------------------------------------------------------------------
void 
OsiCbcSolverInterface::deleteRows(const int num, const int * rowIndices)
{
  agentPtr_->solver()->deleteRows(num,rowIndices);
}

//#############################################################################
// Methods to input a problem
//#############################################################################

void
OsiCbcSolverInterface::loadProblem(const CoinPackedMatrix& matrix,
				   const double* collb, const double* colub,   
				   const double* obj,
				   const double* rowlb, const double* rowub)
{
  agentPtr_->solver()->loadProblem(matrix,collb,colub,obj,rowlb,rowub);
}

//-----------------------------------------------------------------------------

void
OsiCbcSolverInterface::assignProblem(CoinPackedMatrix*& matrix,
				     double*& collb, double*& colub,
				     double*& obj,
				     double*& rowlb, double*& rowub)
{
  agentPtr_->solver()->assignProblem(matrix,collb,colub,obj,rowlb,rowub);
}

//-----------------------------------------------------------------------------

void
OsiCbcSolverInterface::loadProblem(const CoinPackedMatrix& matrix,
				   const double* collb, const double* colub,
				   const double* obj,
				   const char* rowsen, const double* rowrhs,   
				   const double* rowrng)
{
  agentPtr_->solver()->loadProblem(matrix,collb,colub,obj,rowsen,rowrhs,rowrng);
}

//-----------------------------------------------------------------------------

void
OsiCbcSolverInterface::assignProblem(CoinPackedMatrix*& matrix,
				     double*& collb, double*& colub,
				     double*& obj,
				     char*& rowsen, double*& rowrhs,
				     double*& rowrng)
{
  agentPtr_->solver()->assignProblem(matrix,collb,colub,obj,rowsen,rowrhs,rowrng);
}

//-----------------------------------------------------------------------------

void
OsiCbcSolverInterface::loadProblem(const int numcols, const int numrows,
				   const CoinBigIndex * start, const int* index,
				   const double* value,
				   const double* collb, const double* colub,
				   const double* obj,
				   const double* rowlb, const double* rowub)
{
  agentPtr_->solver()->loadProblem(numcols,numrows,start,index,value,
                                   collb,colub,obj,rowlb,rowub);
}
//-----------------------------------------------------------------------------

void
OsiCbcSolverInterface::loadProblem(const int numcols, const int numrows,
				   const CoinBigIndex * start, const int* index,
				   const double* value,
				   const double* collb, const double* colub,
				   const double* obj,
				   const char* rowsen, const double* rowrhs,   
				   const double* rowrng)
{
  agentPtr_->solver()->loadProblem(numcols,numrows,start,index,value,
                                   collb,colub,obj,rowsen,rowrhs,rowrng);
}

//-----------------------------------------------------------------------------
// Write mps files
//-----------------------------------------------------------------------------

void OsiCbcSolverInterface::writeMps(const char * filename,
				     const char * extension,
				     double objSense) const
{
  agentPtr_->solver()->writeMps(filename,extension,objSense);
}

int 
OsiCbcSolverInterface::writeMpsNative(const char *filename, 
		  const char ** rowNames, const char ** columnNames,
		  int formatType,int numberAcross,double objSense) const 
{
  return agentPtr_->solver()->writeMpsNative(filename, rowNames, columnNames,
			       formatType, numberAcross,objSense);
}

//#############################################################################
// Constructors, destructors clone and assignment
//#############################################################################
//-------------------------------------------------------------------
// Default Constructor 
//-------------------------------------------------------------------
OsiCbcSolverInterface::OsiCbcSolverInterface (OsiSolverInterface * solver,
                                              CbcStrategy * strategy)
:
OsiSolverInterface()
{
  if (solver) {
    agentPtr_=new CbcModel(*solver);
  } else {
    OSICBC_DFLT_SOLVER solverDflt;
    agentPtr_=new CbcModel(solverDflt);
  }
  if (strategy) {
    agentPtr_->setStrategy(*strategy);
  } else {
    CbcStrategyDefault defaultStrategy;
    agentPtr_->setStrategy(defaultStrategy);
  }
}

//-------------------------------------------------------------------
// Clone
//-------------------------------------------------------------------
OsiSolverInterface * OsiCbcSolverInterface::clone(bool CopyData) const
{
   if (CopyData) {
      return new OsiCbcSolverInterface(*this);
   } else {
      return new OsiCbcSolverInterface();
   }
}


//-------------------------------------------------------------------
// Copy constructor 
//-------------------------------------------------------------------
OsiCbcSolverInterface::OsiCbcSolverInterface (
                  const OsiCbcSolverInterface & rhs)
:
OsiSolverInterface(rhs)
{
  assert (rhs.agentPtr_);
  agentPtr_ = new CbcModel(*rhs.agentPtr_);
}
    

//-------------------------------------------------------------------
// Destructor 
//-------------------------------------------------------------------
OsiCbcSolverInterface::~OsiCbcSolverInterface ()
{
  delete agentPtr_;
}

//-------------------------------------------------------------------
// Assignment operator 
//-------------------------------------------------------------------
OsiCbcSolverInterface &
OsiCbcSolverInterface::operator=(const OsiCbcSolverInterface& rhs)
{
  if (this != &rhs) {    
    OsiSolverInterface::operator=(rhs);
    delete agentPtr_;
    agentPtr_=new CbcModel(*rhs.agentPtr_);
  }
  return *this;
}

//#############################################################################
// Applying cuts
//#############################################################################

void OsiCbcSolverInterface::applyRowCut( const OsiRowCut & rowCut )
{
  agentPtr_->solver()->applyRowCuts(1,&rowCut);
}
/* Apply a collection of row cuts which are all effective.
   applyCuts seems to do one at a time which seems inefficient.
*/
void 
OsiCbcSolverInterface::applyRowCuts(int numberCuts, const OsiRowCut * cuts)
{
  agentPtr_->solver()->applyRowCuts(numberCuts,cuts);
}
/* Apply a collection of row cuts which are all effective.
   applyCuts seems to do one at a time which seems inefficient.
*/
void 
OsiCbcSolverInterface::applyRowCuts(int numberCuts, const OsiRowCut ** cuts)
{
  agentPtr_->solver()->applyRowCuts(numberCuts, cuts);
}
//-----------------------------------------------------------------------------

void OsiCbcSolverInterface::applyColCut( const OsiColCut & cc )
{
  const double * lower = agentPtr_->solver()->getColLower();
  const double * upper = agentPtr_->solver()->getColUpper();
  const CoinPackedVector & lbs = cc.lbs();
  const CoinPackedVector & ubs = cc.ubs();
  int i;

  for ( i=0; i<lbs.getNumElements(); i++ ) {
    int iCol = lbs.getIndices()[i];
    double value = lbs.getElements()[i];
    if ( value > lower[iCol] )
      agentPtr_->solver()->setColLower(iCol, value);
  }
  for ( i=0; i<ubs.getNumElements(); i++ ) {
    int iCol = ubs.getIndices()[i];
    double value = ubs.getElements()[i];
    if ( value < upper[iCol] )
      agentPtr_->solver()->setColUpper(iCol, value);
  }
}
/* Read an mps file from the given filename (defaults to Osi reader) - returns
   number of errors (see OsiMpsReader class) */
int 
OsiCbcSolverInterface::readMps(const char *filename,
			       const char *extension ) 
{
  return agentPtr_->solver()->readMps(filename,extension);
}
// Get pointer to array[getNumCols()] of primal solution vector
const double * 
OsiCbcSolverInterface::getColSolution() const 
{ 
  return agentPtr_->solver()->getColSolution();
}
  
// Get pointer to array[getNumRows()] of dual prices
const double * 
OsiCbcSolverInterface::getRowPrice() const
{ 
  return agentPtr_->solver()->getRowPrice();
}
  
// Get a pointer to array[getNumCols()] of reduced costs
const double * 
OsiCbcSolverInterface::getReducedCost() const 
{ 
  return agentPtr_->solver()->getReducedCost();
}

/* Get pointer to array[getNumRows()] of row activity levels (constraint
   matrix times the solution vector */
const double * 
OsiCbcSolverInterface::getRowActivity() const 
{ 
  return agentPtr_->solver()->getRowActivity();
}
double 
OsiCbcSolverInterface::getObjValue() const 
{
  return agentPtr_->solver()->getObjValue();
}

/* Set an objective function coefficient */
void 
OsiCbcSolverInterface::setObjCoeff( int elementIndex, double elementValue )
{
  agentPtr_->solver()->setObjCoeff(elementIndex,elementValue);
}

/* Set a single column lower bound<br>
   Use -DBL_MAX for -infinity. */
void 
OsiCbcSolverInterface::setColLower( int elementIndex, double elementValue )
{
  agentPtr_->solver()->setColLower(elementIndex,elementValue);
}
      
/* Set a single column upper bound<br>
   Use DBL_MAX for infinity. */
void 
OsiCbcSolverInterface::setColUpper( int elementIndex, double elementValue )
{
  agentPtr_->solver()->setColUpper(elementIndex,elementValue);
}

/* Set a single column lower and upper bound */
void 
OsiCbcSolverInterface::setColBounds( int elementIndex,
				     double lower, double upper )
{
  agentPtr_->solver()->setColBounds(elementIndex,lower,upper);
}
void OsiCbcSolverInterface::setColSetBounds(const int* indexFirst,
					    const int* indexLast,
					    const double* boundList)
{
  agentPtr_->solver()->setColSetBounds(indexFirst,indexLast,boundList);
}
//------------------------------------------------------------------
/* Set a single row lower bound<br>
   Use -DBL_MAX for -infinity. */
void 
OsiCbcSolverInterface::setRowLower( int elementIndex, double elementValue ) {
  agentPtr_->solver()->setRowLower(elementIndex,elementValue);
}
      
/* Set a single row upper bound<br>
   Use DBL_MAX for infinity. */
void 
OsiCbcSolverInterface::setRowUpper( int elementIndex, double elementValue ) {
  agentPtr_->solver()->setRowUpper(elementIndex,elementValue);
}
    
/* Set a single row lower and upper bound */
void 
OsiCbcSolverInterface::setRowBounds( int elementIndex,
	      double lower, double upper ) {
  agentPtr_->solver()->setRowBounds(elementIndex,lower,upper);
}
//-----------------------------------------------------------------------------
void
OsiCbcSolverInterface::setRowType(int i, char sense, double rightHandSide,
				  double range)
{
  agentPtr_->solver()->setRowType(i,sense,rightHandSide,range);
}
//-----------------------------------------------------------------------------
void OsiCbcSolverInterface::setRowSetBounds(const int* indexFirst,
					    const int* indexLast,
					    const double* boundList)
{
  agentPtr_->solver()->setRowSetBounds(indexFirst,indexLast,boundList);
}
//-----------------------------------------------------------------------------
void
OsiCbcSolverInterface::setRowSetTypes(const int* indexFirst,
				      const int* indexLast,
				      const char* senseList,
				      const double* rhsList,
				      const double* rangeList)
{
  agentPtr_->solver()->setRowSetTypes(indexFirst,indexLast,senseList,rhsList,rangeList);
}
// Set a hint parameter
bool 
OsiCbcSolverInterface::setHintParam(OsiHintParam key, bool yesNo,
                                    OsiHintStrength strength,
                                    void * otherInformation) 
{
  return agentPtr_->solver()->setHintParam(key,yesNo, strength, otherInformation);
}

// Get a hint parameter
bool 
OsiCbcSolverInterface::getHintParam(OsiHintParam key, bool & yesNo,
                                    OsiHintStrength & strength,
                                    void *& otherInformation) const
{
  return agentPtr_->solver()->getHintParam(key,yesNo, strength, otherInformation);
}

// Get a hint parameter
bool 
OsiCbcSolverInterface::getHintParam(OsiHintParam key, bool & yesNo,
                                    OsiHintStrength & strength) const
{
  return agentPtr_->solver()->getHintParam(key,yesNo, strength);
}


int 
OsiCbcSolverInterface::getNumCols() const
{
  return agentPtr_->solver()->getNumCols();
}
int 
OsiCbcSolverInterface::getNumRows() const
{
  return agentPtr_->solver()->getNumRows();
}
int 
OsiCbcSolverInterface::getNumElements() const
{
  return agentPtr_->solver()->getNumElements();
}
const double * 
OsiCbcSolverInterface::getColLower() const
{
  return agentPtr_->solver()->getColLower();
}
const double * 
OsiCbcSolverInterface::getColUpper() const
{
  return agentPtr_->solver()->getColUpper();
}
const double * 
OsiCbcSolverInterface::getRowLower() const
{
  return agentPtr_->solver()->getRowLower();
}
const double * 
OsiCbcSolverInterface::getRowUpper() const
{
  return agentPtr_->solver()->getRowUpper();
}
const double * 
OsiCbcSolverInterface::getObjCoefficients() const 
{
  return agentPtr_->solver()->getObjCoefficients();
}
double 
OsiCbcSolverInterface::getObjSense() const 
{
  return agentPtr_->solver()->getObjSense();
}
double 
OsiCbcSolverInterface::getInfinity() const
{
  return agentPtr_->solver()->getInfinity();
}
int 
OsiCbcSolverInterface::getIterationCount() const 
{
  return agentPtr_->solver()->getIterationCount();
}
void 
OsiCbcSolverInterface::setObjSense(double s )
{
  agentPtr_->setObjSense(s);
}
// Invoke solver's built-in enumeration algorithm
void 
OsiCbcSolverInterface::branchAndBound()
{
  *messageHandler() << "Warning: Use of OsiCbc is deprecated." << CoinMessageEol;
  *messageHandler() << "To enjoy the full performance of Cbc, use the CbcSolver interface." << CoinMessageEol;
  agentPtr_->branchAndBound();
}

/*
  Name discipline support   -- lh, 070328 --

  For safety, there's really nothing to it but to pass each call of an impure
  virtual method on to the underlying solver. Otherwise we just can't know if
  it's been overridden or not.
*/

std::string
OsiCbcSolverInterface::dfltRowColName (char rc, int ndx, unsigned digits) const
{
  return (agentPtr_->solver()->dfltRowColName(rc,ndx,digits)) ;
}

std::string OsiCbcSolverInterface::getObjName (unsigned maxLen) const
{
  return (agentPtr_->solver()->getObjName(maxLen)) ;
}

std::string OsiCbcSolverInterface::getRowName (int ndx, unsigned maxLen) const
{
  return (agentPtr_->solver()->getRowName(ndx,maxLen)) ;
}

const OsiSolverInterface::OsiNameVec &OsiCbcSolverInterface::getRowNames ()
{
  return (agentPtr_->solver()->getRowNames()) ;
}

std::string OsiCbcSolverInterface::getColName (int ndx, unsigned maxLen) const
{
  return (agentPtr_->solver()->getColName(ndx,maxLen)) ;
}

const OsiSolverInterface::OsiNameVec &OsiCbcSolverInterface::getColNames ()
{
  return (agentPtr_->solver()->getColNames()) ;
}

void OsiCbcSolverInterface::setRowNames (OsiNameVec &srcNames,
					 int srcStart, int len, int tgtStart)
{
  agentPtr_->solver()->setRowNames(srcNames,srcStart,len,tgtStart) ;
}

void OsiCbcSolverInterface::deleteRowNames (int tgtStart, int len)
{
  agentPtr_->solver()->deleteRowNames(tgtStart,len) ;
}

void OsiCbcSolverInterface::setColNames (OsiNameVec &srcNames,
					 int srcStart, int len, int tgtStart)
{
  agentPtr_->solver()->setColNames(srcNames,srcStart,len,tgtStart) ;
}

void OsiCbcSolverInterface::deleteColNames (int tgtStart, int len)
{
  agentPtr_->solver()->deleteColNames(tgtStart,len) ;
}

/*
  These last three are the only functions that would normally be overridden.
*/

/*
  Set objective function name.
*/
void OsiCbcSolverInterface::setObjName (std::string name)
{
  agentPtr_->solver()->setObjName(name) ;
}

/*
  Set a row name, to make sure both the solver and OSI see the same name.
*/
void OsiCbcSolverInterface::setRowName (int ndx, std::string name)

{ 
  agentPtr_->solver()->setRowName(ndx,name) ;
}

/*
  Set a column name, to make sure both the solver and OSI see the same name.
*/
void OsiCbcSolverInterface::setColName (int ndx, std::string name)

{ 
  agentPtr_->solver()->setColName(ndx,name) ;
}
// Pass in Message handler (not deleted at end)
void 
OsiCbcSolverInterface::passInMessageHandler(CoinMessageHandler * handler)
{
  OsiSolverInterface::passInMessageHandler(handler);
  if (agentPtr_)
    agentPtr_->passInMessageHandler(handler);
}
// So unit test can find out if NDEBUG set
bool OsiCbcHasNDEBUG() 
{
#ifdef NDEBUG
  return true;
#else
  return false;
#endif
}
