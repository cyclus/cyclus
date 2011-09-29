// MarketModel.h
#if !defined(_MARKETMODEL_H)
#define _MARKETMODEL_H

#include <deque>
#include <set>

#include "Model.h"
#include "Communicator.h"

/// forward declaration to resolve recursion
class Commodity;

//-----------------------------------------------------------------------------
/*
 * The MarketModel class is the abstract class/interface used by all market
 * models
 * 
 * This is all that is known externally about Markets
 */
//-----------------------------------------------------------------------------
class MarketModel : public Model, public Communicator
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /// Default constructor for the MarketModel Class
  MarketModel() {
    setModelType("Market");
    commType_=MARKET_COMM;
  };
  
  /// MarketModels should not be indestructible.
  virtual ~MarketModel() {};
  
  // every model needs a method to initialize from XML
  virtual void init(xmlNodePtr cur);

  // every model needs a method to copy one object to another
  virtual void copy(MarketModel* src);

  /**
   * This drills down the dependency tree to initialize all relevant parameters/containers.
   *
   * Note that this function must be defined only in the specific model in question and not in any 
   * inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src)=0;

  // every model should be able to print a verbose description
  virtual void print();

public:
  /// default MarketModel receiver simply logs the offer/request
  virtual void receiveMessage(Message* msg) 
  { messages_.insert(msg); };

protected:


/* ------------------- */ 


/* --------------------
 * all MARKETMODEL classes have these members
 * --------------------
 */
public:
  /// every market should provide its commodity
  Commodity* getCommodity() { return commodity_; } ;

  // Primary MarketModel methods

  /// Resolve requests with offers
  /**
   *  Primary funcation of a Market is to resolve the set of 
   *  requests with the set of offers.
   */
  virtual void resolve() = 0;

  /// Execute list of shipments
  /**
   *  Once market is resolved, go through the deque/queue and 
   *  execute the shipments.
   */
  virtual void executeOrderQueue();

protected: 
  /// every market has a commodity
  Commodity* commodity_;

  /// every market collects offers & requests
  std::set<Message*> messages_;

  /// every market generates a set of orders
  std::deque<Message*> orders_;
  int firmOrders_;
  

/* ------------------- */ 
  
};

#endif



