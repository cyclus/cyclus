// MarketModel.h
#if !defined(_MARKETMODEL_H)
#define _MARKETMODEL_H

#include <deque>
#include <set>
#include <list>

#include "Model.h"
#include "Communicator.h"

/**
   @brief Markets are used to allocate transactions between agents. Each 
   Market is associated with one resource or commodity.
   
   @section introduction Introduction
   The MarketModel type plays a primary role in Cyclus. A MarketModel market 
   is where a set of offers and requests are collected from facilities and a 
   set of shipments of material is derived from matching these offers and 
   requests. The algorithm for performing this matching of offers with 
   requests is a primary differentiator between different MarketModel 
   implementations.

   Like all model implementations, there are a number of implementations 
   that are distributed as part of the core Cyclus application as well as 
   implementations contributed by third-party developers. The links below 
   describe additional parameters necessary for the complete definition of a 
   market of that implementation.

   @section availableCoreImpl Available Core Implementations
   (None)
   
   @section anticipatedCoreImpl Anticipated Core Implementations
   - NullMarket: Match first offer with first request, defining the transfer 
   quantity as the smaller of the two.
   - GreedyMarket: Match largest requests first with largest remaining 
   offers. Only match complete requests and fail if full request can't be 
   matched.
   - <a href="http://code.google.com/p/cyclus/wiki/NetFlowMarket">
   NetFlowMarket</a>: Use network flow algorithm with arc costs (aka trade 
   affinitites) defined on each facility pair in the market (GENIUS2)

   @section thirdPartyImpl Third-party Implementations
   (None)
 */
class MarketModel : public Model, public Communicator {
/* --------------------
 * THIS class has these members
 * --------------------
 */
 private:
  /**
   * @brief  a list of all markets
   */
  static std::list<MarketModel*> markets_;

 public:
  /**
   * @brief  default constructor
   */
  MarketModel();
  
  /**
   * @brief  MarketModels should not be indestructible.
   */
  virtual ~MarketModel();
  
  /**
   * @brief Queries the list of known markets for one associated with the commodity
   *
   * @param commod a string naming the commodity whose market is of interest
   */
  static MarketModel* marketForCommod(std::string commod);

  /**
   * @brief  every model needs a method to initialize from XML
   */
  virtual void init(xmlNodePtr cur);

  /**
   * @brief  every model needs a method to copy one object to another
   */
  virtual void copy(MarketModel* src);

  /**
   * @brief This drills down the dependency tree to initialize all relevant parameters/containers.
   *
   * Note that this function must be defined only in the specific model in question and not in any 
   * inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src)=0;

  /**
   * @brief  every model should be able to print a verbose description
   */
  virtual void print();

  /**
   * @brief  default MarketModel receiver simply logs the offer/request
   */
  virtual void receiveMessage(msg_ptr msg) 
  { messages_.insert(msg); };

/* ------------------- */ 


/* --------------------
 * all MARKETMODEL classes have these members
 * --------------------
 */
public:
  /**
   * @brief  every market should provide its commodity
   */
  std::string commodity() { return commodity_; } ;

  /**
   * @brief  Primary funcation of a Market is to resolve the set of 
   *  requests with the set of offers.
   */
  virtual void resolve() = 0;

protected: 
  /**
   * @brief  every market has a commodity
   */
  std::string commodity_;

  /**
   * @brief  every market collects offers & requests
   */
  std::set<msg_ptr> messages_;

  /**
   * @brief  every market generates a set of orders
   */
  std::deque<msg_ptr> orders_;

  /**
   * @brief  every market knows its number of firm orders
   */
  int firmOrders_;
  
/* ------------------- */ 
  
};

#endif



