// NullMarket.h
#if !defined(_NULLMARKET_H)
#define _NULLMARKET_H

#include <map>
#include <deque>

#include "MarketModel.h"

/**
   The NullMarket class inherits from the MarketModel class and is 
   dynamically loaded by the Model class when requested. 
    
   This market will take a set of requests and match the biggest 
   requests with the biggest offers first. 
    
   @section intro Introduction 
   The NullMarket is a market type in Cyclus which matches the first 
   offer with the first request for its characteristic commodity. The 
   NullMarket is instantiated at the beginning of the simulation and 
   exists until the end of the simulation. 
    
   @section modelParams Model Parameters 
   NullMarket behavior is comprehensively defined by the following 
   parameters: - Commodity* commod: The type of commodity for which this 
   market accepts offers and requests. 
    
   @section behavior Detailed Behavior 
   The NullMarket starts operation at the beginning of the simulation 
   and ends operation at the end of the simulation. It collects offers 
   and requests for its characteristic commodity and matches offers and 
   requests according to the simplest of algorithms, matching the first 
   offerer with the first requester and setting the amount to be the 
   smallest of the two. Once matches are made, the market dictates the 
   matches back down to the facilities. 
 */
class NullMarket : public MarketModel  
{
/* --------------------
 * all MODEL classes have these public members
 * --------------------
 */
 public:
  /**
     Default constructor 
   */
  NullMarket() {};
  
  /** 
     Destructor 
   */
  virtual ~NullMarket() {};
  
  // different ways to populate an object after creation
  /**
     initialize an object from XML input 
   */
  virtual void init(xmlNodePtr cur)  { MarketModel::init(cur); };

  /**
     initialize an object by copying another 
   */
  virtual void copy(NullMarket* src) { MarketModel::copy(src); } ;

  /**
     This drills down the dependency tree to initialize all relevant 
     parameters/containers.  
     Note that this function must be defined only in the specific model 
     in question and not in any inherited models preceding it. 
      
     @param src the pointer to the original (initialized ?) model to be 
   */
  virtual void copyFreshModel(Model* src){ copy(dynamic_cast<NullMarket*>(src));};

  /** 
     Prints information about this model. 
   */
  virtual std::string str()       { MarketModel::str();   } ;

/* -------------------- */


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
 public:
  /**
     The market receives an offer or request that has been passed from 
     the facility to the institution to the region. 
      
     @param msg is a pointer to the message, an Message object 
   */
  virtual void receiveMessage(msg_ptr msg);

/* -------------------- */


/* --------------------
 * all MARKETMODEL classes have these members
 * --------------------
 */
 public:
  /**
     This instructs the market to make matches with the offers and 
     requests it has on hand. 
   */
  virtual void resolve();

/* -------------------- */


/* --------------------
 * _THIS_ MARKETMODEL class has these members
 * --------------------
 */
 private:
  /**
     The messages that have been indexed 
   */
  typedef std::pair<double,msg_ptr> indexedMsg;

  /**
     The messages of both offer and request types that have been sorted 
     according to thie size. 
   */
  typedef std::multimap<double,msg_ptr> sortedMsgList;
  
  /** 
     The requests that have been sorted according to their size. 
   */
  sortedMsgList requests_;

  /**
     The offers that have been sorted according to their size. 
   */
  sortedMsgList offers_;

  /**
     The set of pointers to offers that have been matched. 
   */
  std::set<msg_ptr> matchedOffers_;
  
  /**
     This function adds an Message object to the list of matchedOffers 
      
     @param msg a pointer to the message to add 
   */
  void add(msg_ptr msg);

  /**
     A boolean that keeps track of whether requests have been matched. 
     True if matched, false if umatched. 
   */
  bool match_request(sortedMsgList::iterator request);

  /**
     A boolean that keeps track of whether requests have been rejected. 
     True if rejected, false if not rejected. 
   */
  void reject_request(sortedMsgList::iterator request);

  /**
     This function processes the information in a request. 
   */
  void process_request();

};

#endif
