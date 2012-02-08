// GreedyMarket.h
#if !defined(_GREEDYMARKET_H)
#define _GREEDYMARKET_H

#include <map>
#include <set>

#include "MarketModel.h"


/**
   @brief The GreedyMarket class inherits from the MarketModel class and is 
   dynamically loaded by the Model class when requested.

   @section introduction Introduction
   The GreedyMarket is a market type in Cyclus which, when given a list of 
   offers and requests for its characteristic commodity, matches the largest 
   requests first with largest remaining offers. It only matches complete 
   requests and fails only if the full request can't be matched. When these 
   matches are made, it issues an order to the sending and receiving 
   facilities that the offer has been matched with a request. The 
   facilities then conduct the appropriate shipment based on the order 
   message issued to them by the market.

   @section modelParams Model Parameters
   GreedyMarket behavior is comprehensively defined by the following parameter:
   - Commodity* commod : The characteristic commodity of this market

   @section behavior Detailed Behavior
   All GreedyMarket instantiations occur at the start of the simulation. At 
   each Tick of the timer, the GreedyMarket will receive any offer and
   request messages associated with its characteristic commodity. When the 
   list of offers and requests are compiled, it handles the offers and 
   requests by descending size. Unless the largest offer is incapable of 
   filling the largest request, the two are matched. Any remaining quantity 
   of the large offer is refiled in the ordered list of offers according to 
   its new size. If the largest offer is smaller than the largest request, 
   the GreedyMarket tries again to match it with the second largest request, 
   and so on. The GreedyMarket proceeds through the ordered list of offers 
   until the largest offer is not big enough to fill the smallest outstanding 
   request.
*/   

class GreedyMarket : public MarketModel {
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
   * This drills down the dependency tree to initialize all relevant parameters/containers.
   *
   * Note that this function must be defined only in the specific model in question and not in any 
   * inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src) { copy(dynamic_cast<GreedyMarket*>(src)); } ;

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
   * @param msg is a pointer to the message, an Message object
   */
  virtual void receiveMessage(msg_ptr msg);
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
  typedef std::pair<double,msg_ptr> indexedMsg;

  /**
   * The messages of both offer and request types that have been sorted
   * according to thie size.
   */
  typedef std::multimap<double,msg_ptr> sortedMsgList;
  
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
  std::set<msg_ptr> matchedOffers_;
  
  /**
   * This function adds an Message object to the list of matchedOffers
   *
   * @param msg a pointer to the message to add
   */
  void add(msg_ptr msg);

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

#endif
