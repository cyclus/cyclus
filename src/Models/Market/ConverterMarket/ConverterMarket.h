// ConverterMarket.h
#if !defined(_CONVERTERMARKET_H)
#define _CONVERTERMARKET_H
#include <iostream>
#include "Logger.h"
#include <map>
#include <deque>

#include "MarketModel.h"
#include "ConverterModel.h"

/**
   @brief
   The ConverterMarket class inherits from the MarketModel 
   class and is dynamically loaded by the Model class when requested.
   
   @section introduction Introduction
   The ConverterMarket is a market type in Cyclus which receives requests 
   and offers for two different commodities, calls an appropriate converter 
   model, and matches appropriate offers and requests according to a scheme 
   much like the GreedyMarket. Either the offer or request must be for the 
   characteristic commodity of that Market. The ConverterMarket is 
   instantiated at the beginning of the simulation and exists until 
   the end of the simulation.

   @section modelParams Model Parameters
   ConverterMarket behavior is comprehensively defined by the following 
   parameters:
   - list<Commodity*> commods: The types of commodity for which this 
   market accepts offers and requests.
   - ConverterModel* converter: The converter that will be called to 
   translate an offer or request in one commodity into another 
   equivalent commodity.
      
   @section detailedBehavior Detailed Behavior
   The ConverterMarket starts operation at the beginning of the simulation 
   and ends operation at the end of the simulation. It collects offers and 
   requests for its characteristic commodities. It employs a converter model 
   to make the appropiate conversions into a single commodity. Once all 
   requests and offers are of the same commodity type, it matches offers and 
   requests according to the greedy algorithm, matching the largest request 
   with the first offers until it is filled and proceeding to fill other 
   requests only if the first large request is filled or is determined to be 
   too large to fill with all offers. Once matches are made, the market 
   dictates the matches back down to the facilities.
*/

class ConverterMarket : public MarketModel  
{
/* --------------------
 * all MODEL classes have these public members
 * --------------------
 */

public:
  /**
   * Default constructor
   */
  ConverterMarket() {};
  
  /** 
   * Destructor
   */
  ~ConverterMarket() {};
  
  // different ways to populate an object after creation
  /// initialize an object from XML input
  virtual void init(xmlNodePtr cur);

  /// initialize an object by copying another
  virtual void copy(ConverterMarket* src) ;

  /**
   * This drills down the dependency tree to initialize all relevant parameters/containers.
   *
   * Note that this function must be defined only in the specific model in question and not in any 
   * inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src);

  /** 
   * Prints information about this model.
   */
  virtual void print() ;

/* -------------------- */

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */

  /**
   * The market receives an offer or request that has been passed from the
   * facility to the institution to the region.
   *
   * @param msg is a pointer to the message, an Message object
   */
  virtual void receiveMessage(Message* msg);
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
public:
  /**
   * Returns a pointer to the converter model this market relies on.
   * 
   * @return converter
   */
  ConverterModel* getConverter();

private:
  /**
   * The messages that have been indexed
   */
  typedef pair<double,Message*> indexedMsg;

  /**
   * The messages of both offer and request types that have been sorted
   * according to thie size.
   */
  typedef multimap<double,Message*> sortedMsgList;
  
  /** 
   * The requests that have been sorted according to their size.
   */
  sortedMsgList requests_;

  /**
   * The offers that have been sorted according to their size.
   */
  sortedMsgList offers_;

  /**
   * The set of pointers to offers that have been matched.
   */
  set<Message*> matchedOffers_;
  
  /**
   * This function adds an Message object to the list of matchedOffers
   *
   * @param msg a pointer to the message to add
   */
  void add(Message* msg);

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

  /**
   * The commodity for which the market receives offers
   */
  Commodity* offer_commod_;

  /**
   * The commodity for which the market receives requests
   */
  Commodity* req_commod_;

  /**
   * The name of the converter model that this market relies on to make conversions.
   */
  string conv_name_;

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
extern "C" Model* construct() {
  return new ConverterMarket();
}

extern "C" void destruct(Model* p) {
  delete p;
}

/* -------------------- */

#endif
