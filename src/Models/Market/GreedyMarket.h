// GreedyMarket.h
#if !defined(_GREEDYMARKET_H)
#define _GREEDYMARKET_H
#include <iostream>
#include <map>
#include <deque>

#include "MarketModel.h"

/**
 * The GreedyMarket class inherits from the MarketModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This market will take a set of requests and match the biggest requests with
 * the biggest offers first.
 *
 */
class GreedyMarket : public MarketModel  
{
/* --------------------
 * all MODEL classes have these public members
 * --------------------
 */

public:
  /**
   * Default constructor
   */
  GreedyMarket() {};
  
  /** 
   * Destructor
   */
  ~GreedyMarket() {};
  
  // different ways to populate an object after creation
  /// initialize an object from XML input
  virtual void init(xmlNodePtr cur)  { MarketModel::init(cur); };

  /// initialize an object by copying another
  virtual void copy(GreedyMarket* src) { MarketModel::copy(src); } ;

  /** 
   * Prints information about this model.
   */
  virtual void print()       { MarketModel::print();   } ;

/* -------------------- */

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */

  /**
   * The market receives an offer or request that has been passed from the
   * facility to the institution to the region.
   *
   * @param msg is a pointer to the message, an OfferRequest object
   */
  virtual void receiveOfferRequest(OfferRequest* msg);
/* -------------------- */


/* --------------------
 * all MARKETMODEL classes have these members
 * --------------------
 */

  /**
   * This instructs the market to make matches with the offers and requests it
   * has on hand.
   */
  virtual void resolve();

/* -------------------- */

/* --------------------
 * _THIS_ MARKETMODEL class has these members
 * --------------------
 */

/* -------------------- */

private:
  /**
   * The messages that have been indexed
   */
  typedef pair<double,OfferRequest*> indexedMsg;

  /**
   * The messages of both offer and request types that have been sorted
   * according to thie size.
   */
  typedef multimap<double,OfferRequest*> sortedMsgList;
  
  /** 
   * The requests that have been sorted according to their size.
   */
  sortedMsgList requests;

  /**
   * The offers that have been sorted according to their size.
   */
  sortedMsgList offers;

  /**
   * The set of pointers to offers that have been matched.
   */
  set<OfferRequest*> matchedOffers;
  
  /**
   * This function adds an OfferRequest object to the list of matchedOffers
   *
   * @param msg a pointer to the message to add
   */
  void add(OfferRequest* msg);

  /**
   * A boolean that keeps track of whether requests have been matched.
   * True if matched, false if umatched.
   */
  bool match_request(sortedMsgList::iterator request);

  /**
   * A boolean that keeps track of whether requests have been rejected.
   * True if rejected, false if not rejected.
   */
  void reject_request(sortedMsgList::iterator request);

  /**
   * This function processes the information in a request.
   */
  void process_request();

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
extern "C" Model* construct() {
  return new GreedyMarket();
}

extern "C" void destruct(Model* p) {
  delete p;
}

/* -------------------- */

#endif
