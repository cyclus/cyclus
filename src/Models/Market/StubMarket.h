// StubMarket.h
#if !defined(_STUBMARKET_H)
#define _STUBMARKET_H
#include <iostream>

#include "MarketModel.h"

/**
 * The StubMarket class inherits from the MarketModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This market will do nothing. This MarketModel is intended as a skeleton to
 * guide the implementation of new MarketModel models.
 *
 */
class StubMarket : public MarketModel  
{
/* --------------------
 * all MODEL classes have these public members
 * --------------------
 */

public:
  /**
   * Default constructor
   */
  StubMarket() {};
  
  /**
   * Default destructor
   */
  ~StubMarket() {};
  
  // different ways to populate an object after creation
  /// initialize an object from XML input
  virtual void init(xmlNodePtr cur) { MarketModel::init(cur); };

  /// initialize an object by copying another
  virtual void copy(StubMarket* src){ MarketModel::copy(src); } ;

  /**
   * Print information about this model.
   */
  virtual void print()      { MarketModel::print();   } ;

/* -------------------- */

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */

/* -------------------- */

/* --------------------
 * all MARKETMODEL classes have these members
 * --------------------
 */
  /// Resolve requests with offers
  /**
   *  Primary funcation of a Market is to resolve the set of 
   *  requests with the set of offers.
   *
   *  In this stub - do nothing!
   */
  virtual void resolve() {};


/* -------------------- */

/* --------------------
 * _THIS_ MARKETMODEL class these members
 * --------------------
 */

/* -------------------- */

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
extern "C" Model* construct() {
  return new StubMarket();
}

extern "C" void destruct(Model* p) {
  delete p;
}

/* -------------------- */

#endif
